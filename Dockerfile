FROM reo7sp/tgbot-cpp

WORKDIR /usr/src/bot
COPY . .
RUN cmake .
RUN make -j4
CMD ./never_forget_bot
