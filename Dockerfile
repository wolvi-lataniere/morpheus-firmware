FROM zephyrprojectrtos/zephyr-build:v0.23.3 as builder

USER user
RUN mkdir -p /home/user/workdir/morpheus/src
WORKDIR /home/user/workdir
RUN sudo chown -R user:user /workdir
RUN west init /home/user/workdir
ENV ZEPHYR_BASE=/home/user/workdir/zephyr
RUN west update

WORKDIR /home/user/workdir/morpheus

COPY app.overlay CMakeLists.txt Kconfig prj.conf ./
COPY ./src/* ./src/

ENV CMAKE_PREFIX_PATH=/home/user/workdir/zephyr/share/zephyr-package/cmake/
RUN source /home/user/workdir/zephyr/zephyr-env.sh
RUN west build

FROM busybox

WORKDIR /data
COPY --from=builder /home/user/workdir/morpheus/build/zephyr/zephyr.uf2 /data

CMD ["sh"]
