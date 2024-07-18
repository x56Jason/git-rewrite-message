# Rewrite Commit Message

## Install

    $ make
    $ make install

It will install 'git-message' and 'git-message-rewrite.pl' into ~/bin/, and template files into ~/.config/git-message/.

## Usage

### 1. Edit template

First we need to choose a template, and edit corresponding template file in ~/.config/git-message/.

Currently we support 2 templates: baseline, openeuler.

So we need to edit baseline.template or openeuler.template accordingly.

### 2. Set template environment variable

Before rewrite commit messages, we need to set following template environment variable.

    $ export GIT_MSG_TEMPLATE=baseline

or

    $ export GIT_MSG_TEMPLATE=openeuler

### 3. Rewrite commit message according to template

    $ git filter-branch -f --msg-filter 'git-message-rewrite.pl' -- xxxxxxxxxxxx..HEAD

Please note, 'git filter-branch' is a dangerous command, we'd better to backup the repo before run this command, otherwise the repo may be corrupted.
