#! /bin/sh

# On Mac OSX, you don't want to run SonarQube but to exec the build command directly.
if [ ${TRAVIS_OS_NAME} != 'linux' ]  
then
  exec "$@"
fi
# Passed this point, we are on Linux (exec never returns)

# Be verbose and fail fast
set -ex

#get the current git branch name
branch_name=$(git rev-parse --abbrev-ref HEAD)
if [ "$branch_name" != "master" ]
then
	cat "\nsonar.branch.name=$branch_name \n" >> ./sonar-project.properties
fi

