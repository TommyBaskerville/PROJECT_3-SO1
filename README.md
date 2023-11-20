# PROJECT_3-SO1
Objetivo: Por cada uno de los problemas descritos a continuación, realizar el programa correspondiente en lenguaje C, haciendo uso de semáforos para resolver el problema de sincronizacón, además, implementar las técnicas de IPC (Inter-Process Communication) así como, el paso de mensajes. Para este proyecto no se utilizarán Pthreads (hilos de POXIS), todo tiene que ser con procesos. 
Adicionalmente, se deberá implementar un menú para que el usuario elija que problema desea ejecutar. 
 
Filósofos comensales (cena de los 5 filósofos) 
 
Descripción del problema:  
 
Cinco filósofos se sientan a la mesa, cada uno con un plato de espaghetti. El espaghetti es  tan escurridizo que un filósofo necesita dos tenedores para comerlo. Entre cada dos platos  hay un tenedor. 
La vida de un filósofo consta de periodos alternos de comer y pensar. Cuando un filósofo  tiene hambre, intenta obtener un tenedor para su mano derecha, y otro para su mano  izquierda, cogiendo uno a la vez y en cualquier orden. Si logra obtener los dos tenedores,  come un rato y después deja los tenedores y continúa pensando. La pregunta clave es: ¿puede el lector escribir un programa para cada filósofo que permita comer equitativamente a  los filósofos y no se interbloquee? 
 
Desarrollo: Realizar el programa en lenguaje C haciendo uso de comunicación y sincronización entre procesos, para este ejercicio ustedes definirán el recurso compartido para simular la exclusión mutua. 
 
Lectores y escritores 
 
Descripción del problema:  
 
El problema de los lectores y escritores es un clásico en el campo de la concurrencia y se plantea en el contexto de sistemas operativos y programación concurrente. El objetivo es gestionar el acceso a un recurso compartido, que en este caso es un conjunto de datos, por parte de múltiples procesos que actúan como lectores o escritores. 
 
El problema se puede describir de la siguiente manera: 
 
Varios lectores pueden leer simultáneamente. 
Solo un escritor puede escribir a la vez. 
Ningún lector debe ser bloqueado a menos que un escritor esté escribiendo. 
Ningún escritor debe ser bloqueado a menos que otro escritor o lector esté escribiendo o leyendo. 
 
El reto radica en garantizar que los lectores y escritores accedan al recurso compartido de manera segura, evitando condiciones de carrera y asegurando la consistencia de los datos. Es decir, se busca evitar situaciones en las cuales dos o más procesos intentan modificar o leer los datos al mismo tiempo, lo que podría resultar en resultados inesperados o incorrectos. 
 
Una manera común de implementar el problema del Lector y Escritor en C  
 
Desarrollo: Para implementar este programa, utilizarán IPC (Inter-Process Communication) mediante el uso de semáforos y memoria compartida. Para este problema, el recurso compartido será un archivo (FILE) en el cual varios lectores pueden leer del archivo pero cuando sea turno de los escritores uno solo puede tomar el recurso y escribir en el. Deberán resolver el problema de sincronización, realizar el problema utilizando 3 lectores y dos escritores. 
 
 
Problema del barbero dormilón 
 
Descripción del problema:  
 
Es un clásico problema de sincronización en ciencia de la computación y sistemas operativos. Se plantea la situación en la que hay un barbero que puede estar cortando el pelo a un cliente o dormir si no hay clientes. Los clientes, por otro lado, llegan a la barbería y, si el barbero está ocupado, se sientan en la sala de espera. Si el barbero está durmiendo, despiertan al barbero y le piden un corte de pelo. El problema radica en la sincronización adecuada entre el barbero y los clientes para evitar condiciones de carrera y garantizar la exclusión mutua. 
 
El problema del barbero dormilón se puede describir con los siguientes elementos: 
 
1.     Barbero: Puede estar cortando el pelo a un cliente o dormir si no hay clientes. 
 
2.     Clientes: Llegan a la barbería, si el barbero está durmiendo, lo despiertan. Si el barbero está ocupado, se sientan en la sala de espera. Cuando el barbero termina de cortar el pelo a un cliente, este último se va. 
 
El desafío radica en garantizar que los clientes sean atendidos de manera ordenada y que el barbero no intente cortar el pelo a más de un cliente al mismo tiempo. Se requiere una sincronización cuidadosa para evitar condiciones de carrera y otros problemas relacionados con la concurrencia. 
 
Los ejercicios deben realizarse utilizando utilizando Inter-Process Communication (IPC) y no la función pthread para crear hilos. 
 
Desarrollo: Realizar el programa en lenguaje C haciendo uso de comunicación y sincronización entre procesos, para este ejercicio ustedes definirán el recurso compartido para simular la exclusión mutua y resolver el problema planteado.
