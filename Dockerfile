# Build with
# docker build -t domos-env .

FROM ubuntu:xenial

# For the proxy

# Create user
RUN useradd jedi --create-home --password jedi && echo "jedi:jedi" | chpasswd && adduser jedi sudo

RUN apt-get update
# Some utilities
RUN apt-get install -y build-essential \
  nano \
  vim \
  unzip \
  curl \
  git \
  sudo \
  tig \
  tmux \
  dos2unix \
  lsb-release \
  silversearcher-ag \
  clang \
  cmake \
  google-mock \
# Python
  python \
  python-dev \
  python-setuptools \
# libraries
  libncurses5-dev \
  libc++-dev \
  libc++abi-dev

# pip
RUN easy_install pip

# Google Test/Mock to compile with x86 as target
RUN cd /usr/src/gmock && cmake CMakeLists.txt && make

USER jedi
# dotfile configuration
RUN cd /home/jedi && \
  git clone https://github.com/jdmichaud/dotfiles && \
  rm -fr /home/jedi/.mybashrc && \
  ln -s /home/jedi/dotfiles/.mybashrc /home/jedi/.mybashrc && \
  ln -s /home/jedi/dotfiles/.vimrc /home/jedi/.vimrc && \
  ln -s /home/jedi/dotfiles/.vimrc.local /home/jedi/.vimrc.local && \
  ln -s /home/jedi/dotfiles/.vimrc.local.bundles /home/jedi/.vimrc.local.bundles && \
  ln -s /home/jedi/dotfiles/.tmux.conf /home/jedi/.tmux.conf && \
  ln -s /home/jedi/dotfiles/.git /home/jedi/.git && \
  ln -s /home/jedi/dotfiles/sh /home/jedi/sh

# Force color prompt
RUN sed -i 's/#force_color_prompt=yes/force_color_prompt=yes/g' /home/jedi/.bashrc
