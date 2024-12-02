# Makefile
CMAKE_COMMON_FLAGS ?= -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
CMAKE_DEBUG_FLAGS ?= -DUSERVER_SANITIZE='addr ub'
CMAKE_RELEASE_FLAGS ?=
NPROCS ?= $(shell nproc)
CLANG_FORMAT ?= clang-format
DOCKER = docker
DOCKER_COMPOSE ?= docker-compose

CMAKE_DEBUG_FLAGS += -DCMAKE_BUILD_TYPE=Debug $(CMAKE_COMMON_FLAGS)
CMAKE_RELEASE_FLAGS += -DCMAKE_BUILD_TYPE=Release $(CMAKE_COMMON_FLAGS)


.PHONY: all
all: test-debug test-release

# Run cmake
.PHONY: cmake-debug
cmake-debug:
	cmake -B build_debug $(CMAKE_DEBUG_FLAGS)

.PHONY: cmake-release
cmake-release:
	cmake -B build_release $(CMAKE_RELEASE_FLAGS)

build_debug/CMakeCache.txt: cmake-debug
build_release/CMakeCache.txt: cmake-release

# Build using cmake
.PHONY: build-debug build-release
build-debug build-release: build-%: build_%/CMakeCache.txt
	cmake --build build_$* -j $(NPROCS) --target never_forget_bot

# Test
.PHONY: test-debug test-release
test-debug test-release: test-%: build-%
	cmake --build build_$* -j $(NPROCS) --target never_forget_bot_unittest
	cd build_$* && ((test -t 1 && GTEST_COLOR=1 PYTEST_ADDOPTS="--color=yes" ctest -V) || ctest -V)

# Start the service (via testsuite service runner)
.PHONY: start-debug start-release
start-debug start-release: start-%: build-%
	cmake --build build_$* -v --target never_forget_bot

.PHONY: service-start-debug service-start-release
service-start-debug service-start-release: service-start-%: start-%

# Cleanup data
.PHONY: clean-debug clean-release
clean-debug clean-release: clean-%:
	cmake --build build_$* --target clean

.PHONY: dist-clean
dist-clean:
	rm -rf build_*
	rm -rf tests/__pycache__/
	rm -rf tests/.pytest_cache/

# Install
.PHONY: install-debug install-release
install-debug install-release: install-%: build-%
	cmake --install build_$* -v --component never_forget_bot

.PHONY: install
install: install-release

# Format the sources
.PHONY: format
format:
	find src -name '*pp' -type f | xargs $(CLANG_FORMAT) -i
	find tests -name '*.py' -type f | xargs autopep8 -i

# Set environment for --in-docker-start
export DB_CONNECTION := postgresql://${POSTGRES_USER}:${POSTGRES_PASSWORD}@service-postgres:5432/${POSTGRES_DB}

# Internal hidden targets that are used only in docker environment
--in-docker-start-debug --in-docker-start-release: --in-docker-start-%: install-%
	psql ${DB_CONNECTION} -f ./postgresql/schemas/db-1.sql
	/root/.local/bin/never_forget_bot \
		--config /root/.local/etc/never_forget_bot/static_config.yaml \
		--config_vars /root/.local/etc/never_forget_bot/config_vars.docker.yaml

# Build and run service in docker environment
.PHONY: docker-start-debug docker-start-release
docker-start-debug docker-start-release: docker-start-%:
	$(DOCKER_COMPOSE) run -p 8080:8080 --rm never_forget_bot-container make -- --in-docker-start-$*

.PHONY: docker-start-service-debug docker-start-service-release
docker-start-service-debug docker-start-service-release: docker-start-service-%: docker-start-%

# Start targets makefile in docker environment
.PHONY: docker-cmake-debug docker-build-debug docker-test-debug docker-clean-debug docker-install-debug docker-cmake-release docker-build-release docker-test-release docker-clean-release docker-install-release
docker-cmake-debug docker-build-debug docker-test-debug docker-clean-debug docker-install-debug docker-cmake-release docker-build-release docker-test-release docker-clean-release docker-install-release: docker-%:
	$(DOCKER_COMPOSE) run --rm never_forget_bot-container make $*

# Stop docker container and remove PG data
.PHONY: docker-clean-data
docker-clean-data:
	$(DOCKER_COMPOSE) down -v
	rm -rf ./.pgdata


# Check if .env exists and include it
ifeq ($(shell test -e .env && echo yes),yes)
    include .env
endif

args := $(wordlist 2, 100, $(MAKECMDGOALS))
ifndef args
MESSAGE = "No such command (or you pass two or many targets to ). List of possible commands: make help"
else
MESSAGE = "Done"
endif


HELP_FUN = \
	%help; while(<>){push@{$$help{$$2//'options'}},[$$1,$$3] \
	if/^([\w-_]+)\s*:.*\#\#(?:@(\w+))?\s(.*)$$/}; \
    print"$$_:\n", map"  $$_->[0]".(" "x(20-length($$_->[0])))."$$_->[1]\n",\
    @{$$help{$$_}},"\n" for keys %help; \


# Commands
.PHONY: help
help: ##@Help Show this help
	@echo -e "Usage: make [target] ...\n"
	@perl -e '$(HELP_FUN)' $(MAKEFILE_LIST)


.PHONY: env
env:  ##@Environment Create .env file with variables
	@$(eval SHELL:=/bin/bash)
	@cp .env.example .env

.PHONY: docker-up
docker-up: ##@Application Docker up
	$(DOCKER_COMPOSE) up --remove-orphans

.PHONY: docker-up-d
docker-up-d: ##@Application Docker up detach
	$(DOCKER_COMPOSE) up -d --remove-orphans

.PHONY: docker-build
docker-build: ##@Application Docker build
	$(DOCKER_COMPOSE) build

.PHONY: docker-up-build
docker-up-build: ##@Application Docker up detach with build
	$(DOCKER_COMPOSE) up -d --build --remove-orphans

.PHONY: docker-down
docker-down: ##@Application Docker down
	$(DOCKER_COMPOSE) down

.PHONY: docker-stop
docker-stop: ##@Application Docker stop some app
	$(DOCKER_COMPOSE) stop $(args)

.PHONY: docker-clean
docker-clean: ##@Application Docker prune -f
	$(DOCKER) image prune -f

.PHONY: docker
docker: docker-clean docker-build docker-up-d docker-clean ##@Application Docker prune, up, run and prune

.PHONY: open
open: ##@Docker Open container in docker
	$(DOCKER) exec -it $(args) /bin/bash

.PHONY: docker-run
docker-run: ##@Docker Run sh in paused docker container
	$(DOCKER) run --rm -it --entrypoint bash $(args)

.PHONY: docker-test
docker-test: ##@Docker Run sh in paused docker container
	$(DOCKER_COMPOSE) run --rm bot make test

# New Migrate Command
.PHONY: migrate
migrate: docker-up-d ##@Migration Run database migrations
	@echo "POSTGRES_USER=$(POSTGRES_USER)"
	@echo "POSTGRES_DB=$(POSTGRES_DB)"
	@echo "Waiting for PostgreSQL to be ready..."
	attempts=0; \
	max_attempts=30; \
	while ! $(DOCKER_COMPOSE) exec postgres pg_isready -U $(POSTGRES_USER) -d $(POSTGRES_DB) > /dev/null 2>&1; do \
		if [ $$attempts -ge $$max_attempts ]; then \
			echo "PostgreSQL is not ready after $$max_attempts attempts. Exiting."; \
			exit 1; \
		fi; \
		echo "PostgreSQL is not ready yet. Waiting... ($$attempts/$$max_attempts)"; \
		sleep 2; \
		attempts=`expr $$attempts + 1`; \
	done
	@echo "PostgreSQL is ready. Running migrations..."
	@for file in migrations/*.sql; do \
		echo "Applying $$file..."; \
		cat $$file | $(DOCKER_COMPOSE) exec -T postgres psql -U $(POSTGRES_USER) -d $(POSTGRES_DB) || { echo "Migration $$file failed."; exit 1; }; \
	done
	@echo "Migrations completed successfully."
