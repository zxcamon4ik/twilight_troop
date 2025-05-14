# Use Ubuntu so apt works out of the box
FROM ubuntu:22.04

ARG WORKDIR=/wd
ARG USER=user

# 1. Set working dir and install system deps as root
WORKDIR ${WORKDIR}
RUN apt-get update \
 && apt-get upgrade -y \
 && apt-get install -y \
      libncurses5-dev \
      libncursesw5-dev \
      ncurses-term \
      build-essential \
      locales \
 && rm -rf /var/lib/apt/lists/*

# Set up locale
RUN locale-gen en_US.UTF-8
ENV LANG en_US.UTF-8
ENV LANGUAGE en_US:en
ENV LC_ALL en_US.UTF-8

# 2. Create & chown before dropping privileges
RUN useradd --system ${USER} \
 && mkdir -p ${WORKDIR} \
 && chown -R ${USER}:${USER} ${WORKDIR}

# 3. Switch to unprivileged user
USER ${USER}

# 4. Copy your entrypoint & start scripts
COPY --chown=${USER}:${USER} docker/services/entrypoint.sh ./entrypoint.sh
COPY --chown=${USER}:${USER} docker/services/start.sh     ./start.sh
RUN chmod +x entrypoint.sh start.sh

# 5. Copy your Makefile, source & data
COPY --chown=${USER}:${USER} Makefile Makefile
COPY --chown=${USER}:${USER} src        src
COPY --chown=${USER}:${USER} data       data

# Set the terminal type
ENV TERM=xterm-256color

ENTRYPOINT ["./entrypoint.sh"]
CMD ["./start.sh"]
