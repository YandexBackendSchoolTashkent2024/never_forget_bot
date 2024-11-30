ifeq ($(shell test -e '.env' && echo -n yes),yes)
	include .env
endif

args := $(wordlist 2, 100, $(MAKECMDGOALS))
ifndef args
MESSAGE = "No such command (or you pass two or many targets to ). List of possible commands: make help"
else
MESSAGE = "Done"
endif

DOCKER = podman
DOCKER_COMPOSE = podman compose

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

