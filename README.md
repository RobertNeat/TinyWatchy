# TinyWatchy

TinyWatchy is an alternative firmware for Watchy, designed with a focus on power efficiency, simplicity, and functionality. It aims to provide a lightweight and powerful experience for Watchy users who prioritize extended battery life and a straightforward user interface.

**This firmware only supports watchy V2 until someone will actually want to use it on <V2**

## Features

- **Power Efficiency**: Optimize battery usage to extend the device's runtime. It can last 5 weeks on 250mAh battery.
- **Simplicity**: Streamlined user interface for easy navigation and a hassle-free experience.
- **Customization**: Tailor the firmware to your preferences with customizable settings.
- **Low Resource Consumption**: Keep resource usage minimal for smoother performance.

## Getting Started

To get started with TinyWatchy, follow these steps:

1. Go to resources and run generate.sh. If you have too old xxd just download latest binary from github
2. Go to src, rename `defines_private.h.template` to `defines_private.h` and modify the contents to your heart's content
3. Now `pio run`

## Contributing

We welcome bug reports, feature requests, and pull requests.

Read [documentation](docs/Index.md).

## License

TinyWatchy is released under the [GPLv3](https://www.gnu.org/licenses/gpl-3.0.en.html). See the [LICENSE](https://github.com/Michal-Szczepaniak/TinyWatchy/blob/master/COPYING) file for details.

## Acknowledgements

This software uses parts of the original [Watchy firmware](https://github.com/sqfmi/Watchy/) licensed under MIT license as well as Bosch driver for BMA423.


## To-do list

```

------------------------------
COMMIT: feat: add watch face
------------------------------
1. dodać nową czcionkę [#]
2. dodać nowy face
	- data
	- godzina
	- stan naładowania
	- ikona synchronizacji
 
 
------------------------------
COMMIT: configuration
------------------------------
1. ustawić wifi domowe
2. wyłączyć bluoetooth na stałe, żeby nie opobiierało prądu
 
 
------------------------------
COMMIT: opt: battery management optimization
------------------------------
1. ustawić odświerzanie czasu co 5 minut (wybudzanie)
--> wyłączyć wybudzanie w godzinach 23:00 - 05:00
2. ustawić synchronizację czasu o 3:00 we poniedziałki, czwartki
3. pomiar napięcia energii tylko po wybudzeniu RTC (jednokrotnie po wybudzeniu)
4. wybudzanie akcelerometrem >> jest jedynie w czasie aktywnym

```