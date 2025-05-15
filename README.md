# Aulas Sustentables

[Ver Demo](https://aulas-sustentables.firebaseapp.com/)

Proyecto final de carrera, Ingenieria en Computación, Departamento de Ciencias e Ingenieria de la Computacion, **Universidad Nacional del Sur**.

### Integrantes
* Fassi Jeremias
* Salazar Gisbert Gabriel

### Capacidades del sistema
   - Determinar si un aula se encuentra o no en uso.
   - Obtener la luminosidad y temperatura del aula.
   - Leer la intensidad de corriente del circuito de luminarias. Se utiliza para detectar si las luces se encuentran encendidas o apagadas.
   - Encender y apagar las luminarias de un aula (sin impedir el funcionamiento manual). 
   - Leer comandos infrarrojos emitidos por cualquier control remoto, almacenarlos y reproducirlos.
   - Apagar el aire acondicionado a través de comando infrarrojo.
   - Actualizar el firmware de cada nodo del sistema mediante OTA.
   - WebServer. Cuando el microcontrolador no tiene credenciales guardadas o no logra conectarse se inicializa en modo AP+STA generando una red wifi nueva e inicializa un pequeño servidor web que permite cargar las credenciales de una red wifi.
   - Panel web para:
      - Visualizar el estado actual de un aula.
      - Accionar remotamente sobre las luces y el aire acondicionado de un aula.
      - Crear y editar aulas, con la posibilidad de segmentarlas por edificios.
      - Configurar la automatización de un aula. Se puede establecer el intervalo de tiempo en el cual se le permite al sistema tomar decisiones.
      - Grabar comandos de un control infrarrojo en un aula específica. El microcontrolador de dicha aula se coloca en modo escucha para recibir el comando.
      - Editar comandos infrarrojos (marca y descripción).
      - Actualizar el firmware mediante OTA. Se permite la subida de un archivo compilado que se retransmite a todos los microcontroladores del sistema para que actualicen su firmware.
      - Logs. Se puede visualizar en tiempo real los logs de cada microcontrolador en funcionamiento.

<br>
<br>

<p align="center">
   <img align="left" src="repoassets/logo-uns-square.png" alt="UNS" width="100" height="100">  
   <img src="repoassets/banderaHD.jpg" alt="flag" width="177" height="100">
   <img align="right" src="repoassets/dcic-logo.jpg" alt="DCIC" width="100" height="100">
</p>
