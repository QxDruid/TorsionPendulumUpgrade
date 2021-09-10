# TorsionPendulumUpgrade
- Программно-Аппаратный комплекс позволяющий оцифровать аналоговые данные с лабораторной установки "Крутильный Маятник"
- Плата на основе контроллера STM32F103 и CCD Toshiba TCD1304AD регистрирует положения отраженного лазерного пучка в реальном времени.
- Массив данных по протоколу USB передается на PC где принимается программой с GUI написанной на Python3 и PyQT5.
- Приложение, по полученным данным строит график распределение интенсивности излучения от положения на CCD и определяет максимум.
- Поддерживается аппроксимация полученных данных.


