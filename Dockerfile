# Build with
# docker build -t ggrep-build .

FROM ubuntu:xenial

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
# For X11 application (i.e. Clion)
  libxrender1 \
  libxtst6 \
  libxi6 \
  libfreetype6 \
  libfontconfig1 \
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
  git clone https://github.com/jdmichaud/sh

# Force color prompt
RUN sed -i 's/#force_color_prompt=yes/force_color_prompt=yes/g' /home/jedi/.bashrc

# Git configuration
RUN git config --global user.email "jean.daniel.michaud@gmail.com" && \
  git config --global user.name "JD"

# Set prompt with image name
RUN echo 'export PS1="`echo $PS1 | sed s/@.h/@ggrep-build/g` "' >> /home/jedi/.profile