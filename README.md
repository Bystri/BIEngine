BIEngine
========

BIEngine - это графический движок с открытым исходным ходом, предназначенный для создания 2D-игр.
Для запуска предоставленного кода требуются: VS 2019; Qt 5.12.12; библиотеки, которые описаны в разделе зависимостей. 

Данный проект реализован в практических целях и может распространяться по MIT-лицензии.


Основной функционал
-------------------
* Менеджер сцен
* Менеджер ресурсов (загрузка разных типов ресурсов по названию и контроль потребляемой памяти)
* Менеджер событий
* Поддерживаемые языки: для расширения функционала движка: C++, для написания игровой логики: Lua
* За симуляцию физики отвечает [Chimpunk](https://chipmunk-physics.net/)
* Объекты (актеры) состоят из компнентов для гибкого создания элементов сцены
	* Transform Component - отвечает за положение объекта на сцене
	* Physics Component - отвечает за включение актера в симуляцию физики
	* Render Component - отвечает за отрисовку актера на экране
	* Camera Component - создает камеру, которой можно управлять через поведение актера
		
Редактор
----------
Редаток для игрового мира написан на QT и позволяет управлять актерами и их компонентами.

![](https://github.com/Bystri/BIEngine/editor_show.gif)
		
Список функций в производстве
----------
* ~~Добавить поддержку камеры в сцене~~ (готово)
* Добавить поддержку 3D
	
	
Поддерживаемые операционные системы
-----------------------------------
* Работоспособность кода проверена только на Windows 10

Используемые библиотеки
-----------------------
Следующие библиотеки должны быть скачаны и помещены в Source/BIEngine/3rdParty

* Общее:
	* Физика: [Chimpunk 7.0.3](https://chipmunk-physics.net/)
	* Привязка функций к событиям: [FastDelegate](https://www.codeproject.com/Articles/7150/Member-Function-Pointers-and-the-Fastest-Possible)
	* Шрифты для интерфейса: [FreeType 2.10.0](https://chipmunk-physics.net/)
	* Lua: [luaplus 5.1](https://github.com/jjensen/luaplus51-all)
	* XML: [tinyxml2](https://github.com/leethomason/tinyxml2)
	* ZIP: [zlib 1.2.12](http://zlib.net/)
	
* Графика:
	* [GLAD](https://glad.dav1d.de/)
	* Создание окон: [GLFW 3.3.6](https://www.glfw.org/)
	* Математика: [GLM 0.9.9.8](https://glm.g-truc.net/0.9.9/index.html)
	* Загрузка спрайтов: [stb](https://github.com/nothings/stb)

* Музыка
	* [irrKlang 1.6](https://www.ambiera.com/irrklang/)
	* [libogg 1.3.0](https://xiph.org/ogg/)
	* [libvorbis 1.3.2](https://xiph.org/vorbis/)
	
Лицензия
--------
[MIT License](https://opensource.org/licenses/MIT).
	
Контакты
--------
* Почта: bystrov.ia.96@gmail.com
