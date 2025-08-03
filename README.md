BIEngine
========

BIEngine - это графический движок с открытым исходным ходом, предназначенный для создания 2D/3D-игр.
Для запуска предоставленного кода требуются: VS 2019; дополнительные библиотеки. 

Данный проект реализован в практических целях и может распространяться по MIT-лицензии, делайте с этим кодом что хотите.


Основной функционал
-------------------
* Менеджер сцен
* Возможность гибкой настройки графического пайплайна
* Менеджер ресурсов (загрузка разных типов ресурсов по названию и контроль потребляемой памяти)
* Менеджер событий
* Инструменты для создания мультиплеерных решений
* Поддерживаемые языки: для расширения функционала движка: C++, для написания игровой логики: Python
* За симуляцию 2D физики отвечает [Chimpunk](https://chipmunk-physics.net/)
* За симуляцию 3D физики отвечает [Bullet](https://github.com/bulletphysics/bullet3)
* Объекты (актеры) состоят из компнентов для гибкого создания элементов сцены
	* Transform Component - отвечает за положение объекта на сцене
	* Physics Component - отвечает за включение актера в симуляцию физики
	* Render Component - отвечает за отрисовку актера на экране
		
Пример
----------
![](https://github.com/Bystri/BIEngine/blob/main/example.gif)
	
Поддерживаемые операционные системы
-----------------------------------
* Работоспособность кода проверена только на Windows 10

Используемые библиотеки
-----------------------
Следующие библиотеки должны быть скачаны и помещены в Source/BIEngine/3rdParty

* Общее:
	* Физика 2D: [Chimpunk 7.0.3](https://chipmunk-physics.net/)
 	* Физика 3D: [Bullet 3.25](https://github.com/bulletphysics/bullet3)
	* Шрифты для интерфейса: [FreeType 2.10.0](https://chipmunk-physics.net/)
	* Python: [pybind11 2.10.3](https://github.com/pybind/pybind11)
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
