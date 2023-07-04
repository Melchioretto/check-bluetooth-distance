# check-bluetooth-distance

ESP32 WROOM-32 - README
Este é um README para utilizar o ESP32 da WROOM-32 em conjunto com o código disponibilizado neste projeto.

Requisitos
Driver: É necessário utilizar o driver disponibilizado no seguinte repositório do GitHub: https://github.com/Melchioretto/check-bluetooth-distance/tree/main/drivers-esp32. Basicamente, execute o arquivo executável "CP210xVCPInstaller_x86.exe" e siga as instruções de instalação.

IDE do Arduino: Certifique-se de ter a IDE do Arduino instalada em seu sistema.

Configuração do Projeto
Abra a IDE do Arduino e vá em "Tools" (Ferramentas) -> "Board" (Placa) e selecione "ESP32 Dev Module".

Em "Tools" (Ferramentas), selecione "Partition Scheme" (Esquema de Partição) e mude para "Huge APP".

No código fornecido, faça as seguintes alterações:
const char* ssid = "coloque o usuário";
const char* pass = "coloque a senha";

Substitua "coloque o usuário" pelo nome do seu Wi-Fi 2.4 GHz e "coloque a senha" pela senha correspondente. É importante observar que o Wi-Fi de 5 GHz não é compatível.
client.publish("melchioretto", message.c_str(), 30);

Você pode definir o nome do tópico MQTT utilizado substituindo "melchioretto" pelo nome desejado.

Visualização das Informações
Existem duas opções para visualizar as informações geradas pelo código:

Monitor Serial: No Arduino, o Monitor Serial funciona como um terminal. Você pode abrir o Monitor Serial na IDE do Arduino para visualizar as informações enviadas pelo ESP32.

Software MQTT: Você pode utilizar um software que seja capaz de se inscrever em tópicos MQTT para visualizar as informações. Escolha um software de sua preferência que suporte MQTT e configure-o para se inscrever no tópico MQTT definido no código.

Conexão dos LEDs
Para fazer os LEDs funcionarem na protoboard, siga as seguintes conexões:

Conecte o fio positivo do LED à porta G13 do ESP32 da WROOM.
Conecte o fio negativo do LED à porta GND do ESP32 da WROOM.
Carregando o Código
Após realizar todas as configurações acima e conectar corretamente o ESP32, faça o upload do código fornecido para o ESP32 usando a IDE do Arduino.

Agora você está pronto para utilizar o ESP32 da WROOM-32 com o código fornecido neste projeto!
