## **Manual de Utilização do Controlador de Aquecimento Segmentado**

### **1. Introdução**
Este sistema utiliza um Arduino Mega para controlar um módulo de relés de 16 canais, sensores de temperatura e sinais PWM para comunicação com a Duet2 + DueX5. Ele é projetado para gerenciar até 16 segmentos de aquecimento, divididos em 4 seções, com controle de temperatura baseado em PID e segurança térmica integrada.

---

### **2. Ligações do Sistema**

#### **2.1. Conexões do Arduino Mega**
- **Relés (16 canais)**:
  - Conecte os pinos digitais do Arduino Mega (`relayPins`) aos pinos de controle do módulo de relés.
  - Pinos utilizados: `22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 53, 51, 49, 47`.
  - Certifique-se de que o módulo de relés utiliza lógica **ativa LOW** (relé ligado quando o pino está em LOW).

- **Sensores de Temperatura (16 sensores)**:
  - Conecte os sensores de temperatura (ex.: termistores) aos pinos analógicos do Arduino Mega (`tempSensors`).
  - Pinos utilizados: `A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15`.
  - Use resistores pull-up de 10kΩ para os termistores.

- **PWM (Duet2 + DueX5)**:
  - **Saída PWM**: Conecte os pinos digitais do Arduino Mega (`pwmOutPins`) às entradas PWM do DueX5.
    - Pinos utilizados: `5, 6, 7, 8`.
  - **Entrada PWM**: Conecte as saídas PWM do Duet2 às entradas PWM do Arduino Mega (`pwmInPins`).
    - Pinos utilizados: `9, 10, 11, 12`.

#### **2.2. Alimentação**
- Certifique-se de que o Arduino Mega, o módulo de relés e os sensores de temperatura compartilham o mesmo **GND**.
- Forneça alimentação adequada para o módulo de relés (ex.: 5V ou 12V, dependendo do modelo).

---

### **3. Comandos Disponíveis**

#### **3.1. Controle de Segmentos**
- **Ativar todos os segmentos**:
  ```
  ON ALL
  ```
  Ativa todos os 16 segmentos de aquecimento.

- **Desativar todos os segmentos**:
  ```
  OFF ALL
  ```
  Desativa todos os 16 segmentos de aquecimento.

- **Ativar um segmento específico**:
  ```
  ON <n>
  ```
  Substitua `<n>` pelo número do segmento (1-16). Exemplo:
  ```
  ON 5
  ```
  Ativa o segmento 5.

- **Desativar um segmento específico**:
  ```
  OFF <n>
  ```
  Substitua `<n>` pelo número do segmento (1-16). Exemplo:
  ```
  OFF 5
  ```
  Desativa o segmento 5.

---

#### **3.2. Configuração de PWM**
- **Configurar faixa de PWM**:
  ```
  SET_PWM_RANGE <minPWM> <maxPWM> <minTemp> <maxTemp>
  ```
  Define os valores mínimos e máximos de PWM e as temperaturas correspondentes. Exemplo:
  ```
  SET_PWM_RANGE 0 2000 0 100
  ```
  Configura o PWM para variar entre 0 e 2000, correspondendo a temperaturas de 0°C a 100°C.

---

#### **3.3. Depuração**
- **Ativar modo de depuração**:
  ```
  DEBUG ON
  ```
  Ativa o modo de depuração, exibindo informações detalhadas no monitor serial.

- **Desativar modo de depuração**:
  ```
  DEBUG OFF
  ```
  Desativa o modo de depuração.

---

#### **3.4. Status do Sistema**
- **Exibir status do sistema**:
  ```
  STATUS
  ```
  Exibe informações sobre os segmentos ativos, temperaturas atuais e setpoints.

- **Exibir lista de comandos disponíveis**:
  ```
  HELP
  ```
  Exibe a lista de comandos suportados.

---

#### **3.5. Segurança Térmica**
- **Resetar estado de segurança térmica**:
  ```
  RESET_SAFETY
  ```
  Reseta o estado de segurança térmica após uma violação de temperatura.

---

### **4. Operação do Sistema**

#### **4.1. Inicialização**
1. Conecte todos os componentes conforme descrito na seção de ligações.
2. Carregue o código no Arduino Mega.
3. Abra o monitor serial no Arduino IDE ou em outro terminal serial (baud rate: **115200**).
4. O sistema exibirá a mensagem:
   ```
   Arduino Mega ready to receive commands from Duet.
   Temperature control system initialized!
   ```

#### **4.2. Controle de Temperatura**
- Configure os setpoints de temperatura no Duet2, que serão enviados ao Arduino via PWM.
- O Arduino ajustará os segmentos de aquecimento com base nos setpoints recebidos e nas leituras dos sensores de temperatura.

#### **4.3. Monitoramento**
- Use o comando `STATUS` para verificar o estado atual do sistema.
- Ative o modo de depuração (`DEBUG ON`) para exibir informações detalhadas, como temperaturas de cada segmento e saídas PID.

#### **4.4. Segurança Térmica**
- O sistema desativará automaticamente todos os segmentos se uma temperatura exceder o limite de segurança (`120°C` por padrão).
- Para resetar o estado de segurança, use o comando `RESET_SAFETY`.

---

### **5. Exemplo de Fluxo de Operação**

1. **Ativar todos os segmentos**:
   ```
   ON ALL
   ```
2. **Configurar faixa de PWM**:
   ```
   SET_PWM_RANGE 0 2000 0 100
   ```
3. **Monitorar o sistema**:
   ```
   STATUS
   ```
4. **Ativar modo de depuração**:
   ```
   DEBUG ON
   ```
5. **Desativar um segmento específico**:
   ```
   OFF 3
   ```
6. **Resetar estado de segurança térmica (se necessário)**:
   ```
   RESET_SAFETY
   ```

---

### **6. Notas Importantes**
- Certifique-se de que os sensores de temperatura estão corretamente calibrados.
- Verifique as conexões elétricas antes de ligar o sistema.
- Não exceda os limites de temperatura para evitar danos ao equipamento.

---