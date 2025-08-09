# Nintendo DS Lite homebrew

Various small test projects to generate graphics for testing.

```
export UID=$(id -u)
export GID=$(id -g)

# Start a docker shell that lets you run `make`:
docker-compose run --rm nds-dev

# Build all projects
docker-compose run --rm nds-build

# Run make clean
docker-compose run --rm nds-build make clean
```
