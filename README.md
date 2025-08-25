# Rewrite Git Commit Messages Using git-filter-branch

## Description

Sometimes you need to rewrite git commit messages in batch by a specific format.

This is a set of script/binary/template to help rewrite git commit messages.

You can define your template to meet your rewriting requirements.

## Install

    $ make
    $ make install

It will install 'git-message', 'git-message-rewrite.pl' and 'git-rewrite-message.sh" into ~/bin/. Please add ~/bin to your PATH.

It also installs template files into ~/.config/git-rewrite-message/.

## Usage

### 1. Edit or add template

First you need to choose a template, and edit corresponding template file in ~/.config/git-rewrite-message/.

Currently 2 templates are supported: baseline, openeuler. You can edit baseline.template or openeuler.template accordingly.

Meanwhile, you can also write your own template. Refer to existing templates for how to create a new template.

### 2. Run git-rewrite-message.sh

    $ git-rewrite-message.sh -t <template-name> <start-commit>

    template-name: baseline or openeuler

Please note, the commits that will be rewritten must be from \<start-commit\> to HEAD, because this script uses 'git filter-branch' to rewrite commit messages, while 'git filter-branch' requires to use current branch.

DANGER: This script uses 'git filter-branch' to rewrite commit messages. 'git filter-branch' is a dangerous command, you'd better to backup the repo before run this command, otherwise the repo may be corrupted.
