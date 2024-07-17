# <p style="text-align: center;">Rewrite Commit Message</p>

## Install

    $ make
    $ make install

It will install git-message into ~/bin/, and template files into ~/.config/git-message/

## Usage

### 1. Edit template

First we need to choose a template, and edit corresponding template file in ~/.config/git-message/

### 2. Set template environment variable

Before rewrite commit messages, we need to set following template environment variable.

    $ export GIT_MSG_TEMPLATE=baseline

or

    $ export GIT_MSG_TEMPLATE=openeuler

### 3. Rewrite commit message according to template

    $ cd linux
    $ git filter-branch -f --msg-filter 'git-message-rewrite.pl' -- xxxxxxxxxxxx..HEAD

