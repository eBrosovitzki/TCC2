0.0.1

C�digo calibrado para detec��o de quedas.

Servidor NODEJS configurado e recebendo informa��es dos sensores.

P�gina web apresentando informa��es dos 6 eixos dos sensores aceler�metros e girosc�picos, assim como temperatura do ambiente um gr�fico apresentando a amplitude atualizada e em tempo real.

Bot�o ativando o buzzer e led built-in, assim como pronto para enviar alertas via SMS. 

O buzzer e led tamb�m s�o ativados quando os sensores detectam uma queda. 


    Problemas detectados:
      - Na detec��o de quedas e ativa��o de alertas e eventos, o reinicio do disposito � necess�rio para desativa��o dos alertas sonoros e visuais.
      - Dificuldades na desativa��o do buzzer e led atrav�s do bot�o.
      
      
--

0.0.2

Altera��es no c�digo que ativa o buzzer e led atrav�s do bot�o. 

Altera��es no c�digo que ativa o buzer e led atrav�s da detec��o da queda pelos sensores.
    - Agora os alertas sonoro e visual s�o ativados por um per�odo pr� determinado e ent�o s�o desativados automaticamente.
    - O alerta via SMS � enviado apenas uma vez, na detec��o da queda. 
    
--

0.0.3

Substitui��o da fun��o delay() por millis() nas seguintes fun��es:
  - De ativa��o do buzzer
  - De ativa��o do led
  - De envio POST
  
  
--

0.0.4

Adicionado � p�gina web uma se��o que apresenta um breve hist�rico dos alertas de queda detectados pelos sensores, assim como hist�rico de acionamento do bot�o de emerg�ncia, ambos com data e hor�rio.

Alterada l�gica do c�digo para ativa��o do buzzer e led quando os sensores detectarem uma queda.

Problemas encontrados:
   - A fun��o para envio de alertas via SMS est� sendo chamada repetidamente, a cada loop do programa.
   
--

0.0.5

Corrigido problema de envios em loop dos alertas via SMS.

Problemas encontrados:
    - Dificuldades na desativa��o dos alarmes sonoro e visual usando o bot�o.
    
    
--

0.0.6

Montado esquema f�sico para obten��o do n�vel de carga da bateria.

Cria��o da fun��o respons�vel por obter e apresentar informa��es sobre carga da bateria.

Cria��o de fun��o para constru��o do objeto JSON enviado ao NODEjs.

Implementado � p�gina web informa��es sobre o estado atualizado da bateria.

Corre��es no c�digo. Removidos/alterados do c�digo a fun��o delay() e substitu�dos por millis() em alguns casos.

Implementado resposta visual ao in�cio e conex�o � rede. O led pisca enquanto a placa tenta se conectar � rede. Ao conectar, o led se mant�m ligado no estado s�lido, caso contr�rio, o led ficar� desligado. 

Corre��es nas mensagens apresentadas no monitor serial.

Problemas encontrados: 
    - Dificuldades na desativa��o dos alarmes sonoro e visual usando o bot�o.
    - Medi��es inadequadas sobre n�vel de bateria. Necess�rio enviar dados para o NODE e testar com conex�o sem fio.
    
--

0.1.0

Nova inteface gr�fica na p�gina web.

Implementada fun��o millis para controlar fluxo da fun��o makePOST, que envia as informa��es para o servidor NODEjs.

Implementada fun��o para conex�o com servidor NODEjs.

--

0.1.1

Corre��es

Implementado c�lculo de bateria. Dados s�o apresentados na p�gina web.

Problemas encontrados:
    - Problema no c�digo de detec��o de queda. 
    - Problema no envio �nico de alertas via SMS.
    
--

0.1.3

C�digo de ativa��o do buzzer e envio do SMS atrav�s do bot�o foi atualizado. Agora o buzzer ativa por um tempo determinado e desativa automaticamente.

L�gica de ativa��o do bot�o foi atualizada. O bot�o s� ativa o buzzer e o envio dos alertas se pressionado por mais de 3 segundos.

Alterada l�gica de detec��o de quedas. Foi utilizada a biblioteca MPU6050.h para obten��o dos dados e c�lculo de ativa��o dos alertas.