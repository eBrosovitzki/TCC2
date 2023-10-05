0.0.1

Código calibrado para detecção de quedas.

Servidor NODEJS configurado e recebendo informações dos sensores.

Página web apresentando informações dos 6 eixos dos sensores acelerômetros e giroscópicos, assim como temperatura do ambiente um gráfico apresentando a amplitude atualizada e em tempo real.

Botão ativando o buzzer e led built-in, assim como pronto para enviar alertas via SMS. 

O buzzer e led também são ativados quando os sensores detectam uma queda. 


    Problemas detectados:
      - Na detecção de quedas e ativação de alertas e eventos, o reinicio do disposito é necessário para desativação dos alertas sonoros e visuais.
      - Dificuldades na desativação do buzzer e led através do botão.
      
      
--

0.0.2

Alterações no código que ativa o buzzer e led através do botão. 

Alterações no código que ativa o buzer e led através da detecção da queda pelos sensores.
    - Agora os alertas sonoro e visual são ativados por um período pré determinado e então são desativados automaticamente.
    - O alerta via SMS é enviado apenas uma vez, na detecção da queda. 
    
--

0.0.3

Substituição da função delay() por millis() nas seguintes funções:
  - De ativação do buzzer
  - De ativação do led
  - De envio POST
  
  
--

0.0.4

Adicionado à página web uma seção que apresenta um breve histórico dos alertas de queda detectados pelos sensores, assim como histórico de acionamento do botão de emergência, ambos com data e horário.

Alterada lógica do código para ativação do buzzer e led quando os sensores detectarem uma queda.

Problemas encontrados:
   - A função para envio de alertas via SMS está sendo chamada repetidamente, a cada loop do programa.
   
--

0.0.5

Corrigido problema de envios em loop dos alertas via SMS.

Problemas encontrados:
    - Dificuldades na desativação dos alarmes sonoro e visual usando o botão.
    
    
--

0.0.6

Montado esquema físico para obtenção do nível de carga da bateria.

Criação da função responsável por obter e apresentar informações sobre carga da bateria.

Criação de função para construção do objeto JSON enviado ao NODEjs.

Implementado à página web informações sobre o estado atualizado da bateria.

Correções no código. Removidos/alterados do código a função delay() e substituídos por millis() em alguns casos.

Implementado resposta visual ao início e conexão à rede. O led pisca enquanto a placa tenta se conectar à rede. Ao conectar, o led se mantém ligado no estado sólido, caso contrário, o led ficará desligado. 

Correções nas mensagens apresentadas no monitor serial.

Problemas encontrados: 
    - Dificuldades na desativação dos alarmes sonoro e visual usando o botão.
    - Medições inadequadas sobre nível de bateria. Necessário enviar dados para o NODE e testar com conexão sem fio.
    
--

0.1.0

Nova inteface gráfica na página web.

Implementada função millis para controlar fluxo da função makePOST, que envia as informações para o servidor NODEjs.

Implementada função para conexão com servidor NODEjs.

--

0.1.1

Correções

Implementado cálculo de bateria. Dados são apresentados na página web.

Problemas encontrados:
    - Problema no código de detecção de queda. 
    - Problema no envio único de alertas via SMS.
    
--

0.1.3

Código de ativação do buzzer e envio do SMS através do botão foi atualizado. Agora o buzzer ativa por um tempo determinado e desativa automaticamente.

Lógica de ativação do botão foi atualizada. O botão só ativa o buzzer e o envio dos alertas se pressionado por mais de 3 segundos.

Alterada lógica de detecção de quedas. Foi utilizada a biblioteca MPU6050.h para obtenção dos dados e cálculo de ativação dos alertas.