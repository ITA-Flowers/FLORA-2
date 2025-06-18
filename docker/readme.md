# Docker

## Directories structure

``` text
docker/
├── docker-compose.yml
├── px4_sitl/
│   └── Dockerfile
└── qgc_gui/
    └── Dockerfile
```

## Getting started

### 1. Create docker network

``` bash
docker network create --subnet=172.20.0.0/16 px4net
```

### 2. Build containers

``` bash
docker compose build
```

### 3. Start services

``` bash
docker compose up
```

### 4. AirSim (Windows host)

Make sure that AirSim's `settings.json` contains following lines:

``` json
"ControlIp": "172.20.0.10",
"ControlPort": 14540
```

Next, run the AirSim.

## Tips

### How to get control on PX4 Commander?

1. Enter the PX4 container

``` bash
docker exec -it px4 /bin/bash
```

2. Get into the PX4 dir

``` bash
cd /home/user/PX4-Autopilot
```

3. Run PX4 shell

``` bash
./build/px4_sitl_default/bin/px4 -d /tmp/px4root
```
