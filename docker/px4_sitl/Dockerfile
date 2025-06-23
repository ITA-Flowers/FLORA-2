FROM px4io/px4-dev-simulation-jammy

RUN git clone https://github.com/PX4/PX4-Autopilot.git /home/user/PX4-Autopilot && \
    cd /home/user/PX4-Autopilot && \
    git submodule update --init --recursive

WORKDIR /home/user/PX4-Autopilot

RUN sed -i '/param load/a \
param set COM_ARM_BAT_ACT 0\n\
param set COM_ARM_MAG 0\n\
param set COM_ARM_GPSF 0\n\
param set COM_POSCTL_NAVL 0' \
    /home/user/PX4-Autopilot/ROMFS/px4fmu_common/init.d-posix/rcS

CMD ["make", "px4_sitl_default", "none_iris"]
