#!/bin/bash

git config --global user.name "${GITHUB_ACTOR}"
git config --global user.email "${GITHUB_ACTOR}@users.noreply.github.com"

remote_repo="https://x-access-token:${GITHUB_TOKEN}@github.com/${GITHUB_REPOSITORY}.git"

git remote rm origin
git remote add origin "${remote_repo}"


git branch -D gh-pages
git checkout gh-pages
git pull --rebase
git checkout master
mkdocs gh-deploy --config-file "${GITHUB_WORKSPACE}/mkdocs.yml" --force

#mkdocs gh-deploy --config-file "${GITHUB_WORKSPACE}/mkdocs.yml" --force

      
