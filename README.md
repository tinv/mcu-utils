
# MCU utils

Library containing useful code that can be used in Zephyr projects. It is necessary 
to install necessary tools and libraries. See https://www.zephyrproject.org for more info.

## Creating workspace
``west init -m ssh://git@bitbucket.co.tinv.ch:7999/libs/mcu-utils.git --mr master mcu-utils_workspace``
<br/>
``cd mcu-utils_workspace``
<br/>
``west update``

## Building
In order to build unit tests issue the following command:
``west build -b native_posix tests/unit``

## Run tests
To run the tests: 
``west twister -T tests --integration ``

## References
- https://github.com/nrfconnect/ncs-example-application
- https://github.com/jbr7rr/zephyr-googletest-example/tree/main