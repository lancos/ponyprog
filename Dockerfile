FROM stepanove/qt:5.15.2-desktop

# From v3.1.4 libftdipp1-3 libftdipp1-dev are no more necessary
RUN sudo apt update && sudo apt -y install \
        pkg-config \
        libusb-1.0-0 \
        libusb-1.0-0-dev \
        libftdi1-2 \
        libftdi1-dev \
        fakeroot \
    && sudo apt -y install \
        wget

RUN wget http://archive.ubuntu.com/ubuntu/pool/main/libf/libftdi1/libftdi1-2_1.5-6build5_amd64.deb \
        http://archive.ubuntu.com/ubuntu/pool/main/libf/libftdi1/libftdi1-dev_1.5-6build5_amd64.deb \
    && sudo apt install -y ./libftdi1-2_1.5-6build5_amd64.deb \
        ./libftdi1-dev_1.5-6build5_amd64.deb \
    && rm libftdi1*.deb
