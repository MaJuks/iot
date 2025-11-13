
// IP do Serial Monitor do Arduino após conectar no Wi-Fi

export const API_CONFIG = {
  BASE_URL: 'http://10.37.0.178', // ⚠️ SUBSTITUA pelo IP do seu ESP32

  ENDPOINTS: {
    STATUS: '/status',      // GET - Retorna estatísticas gerais
    LOGS: '/logs',          // GET - Retorna histórico de ativações
    DISTANCE: '/distance',  // GET - Retorna distância atual do sensor
    ON: '/on',              // POST - Liga a lâmpada manualmente
    OFF: '/off'             // POST - Desliga a lâmpada manualmente
  },
  REFRESH_INTERVAL: 3000, // 3 segundos
};


export interface ApiStatus {
  isOn: boolean;                      // Estado atual da lâmpada
  totalActivations: number;           // Total de vezes que foi ligada
  totalTimeOn: number;                // Tempo total ligada em segundos
  uptime: number;                     // Tempo desde que o Arduino foi ligado (segundos)
  currentDistance: number;            // Distância atual do sensor (cm)
  manualMode: boolean;                // Se está em modo manual
  averageTimeOn: number;              // Tempo médio por ativação (segundos)
  currentSessionDuration: number;     // Duração da sessão atual se estiver ligada
}

export interface ApiLog {
  id: number;                         // ID único da ativação
  timestamp: number;                  // Timestamp em millis (do Arduino)
  duration: number;                   // Duração em segundos
  type: 'on' | 'off';                // Tipo de evento
}

export interface ApiLogsResponse {
  logs: ApiLog[];
  total: number;
}

export interface ApiDistanceResponse {
  distance: number;                   // Distância em cm
  unit: string;                       // Unidade ("cm")
  timestamp: number;                  // Timestamp da leitura
}

export interface ApiControlResponse {
  success: boolean;
  message: string;
  isOn: boolean;
}
