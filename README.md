# Controle de Lâmpada

Projeto simples para controlar uma lâmpada via Arduino ESP32.

## Como usar

1. **Configure o Arduino:**
   - Abra `arduino_codigo_completo.ino`
   - Coloque seu Wi-Fi:
   ```cpp
   const char* ssid = "SEU_WIFI";
   const char* password = "SUA_SENHA";
   ```
   - Faça upload para o ESP32
   - Veja o IP no Serial Monitor

2. **Configure o projeto:**
   - Abra `src/config/api.ts`
   - Coloque o IP do Arduino:
   ```typescript
   BASE_URL: 'http://192.168.x.x'
   ```

3. **Rode o projeto:**
   ```bash
   npm install
   npm run dev
   ```

## Funcionalidades

- Ligar/Desligar a lâmpada
- Ver status atual
- Ver histórico de ativações

## Cores

- Fundo: `#BBDCE5`
- Cards: `#ECEEDF` e `#D9C4B0`
- Botões: `#CFAB8D`

Simples e funcional!
