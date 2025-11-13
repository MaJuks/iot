import {
  API_CONFIG,
  type ApiStatus,
  type ApiLogsResponse,
  type ApiDistanceResponse,
  type ApiControlResponse
} from '../config/api';

/**
 * Busca as estatísticas gerais do sistema
 * Endpoint: GET /status
 */
export async function fetchStatus(): Promise<ApiStatus> {
  try {
    const response = await fetch(`${API_CONFIG.BASE_URL}${API_CONFIG.ENDPOINTS.STATUS}`, {
      method: 'GET',
      mode: 'cors',
    });

    if (!response.ok) {
      throw new Error(`HTTP ${response.status}: ${response.statusText}`);
    }

    const data = await response.json();
    return data;
  } catch (error) {
    console.error('❌ Erro ao buscar status:', error);
    throw error;
  }
}

/**
 * Busca o histórico de ativações
 * Endpoint: GET /logs
 */
export async function fetchLogs(): Promise<ApiLogsResponse> {
  try {
    const response = await fetch(`${API_CONFIG.BASE_URL}${API_CONFIG.ENDPOINTS.LOGS}`, {
      method: 'GET',
      mode: 'cors',
    });

    if (!response.ok) {
      throw new Error(`HTTP ${response.status}: ${response.statusText}`);
    }

    const data = await response.json();
    return data;
  } catch (error) {
    console.error('❌ Erro ao buscar logs:', error);
    throw error;
  }
}

/**
 * Busca a distância atual do sensor
 * Endpoint: GET /distance
 */
export async function fetchDistance(): Promise<ApiDistanceResponse> {
  try {
    const response = await fetch(`${API_CONFIG.BASE_URL}${API_CONFIG.ENDPOINTS.DISTANCE}`, {
      method: 'GET',
      mode: 'cors',
    });

    if (!response.ok) {
      throw new Error(`HTTP ${response.status}: ${response.statusText}`);
    }

    const data = await response.json();
    return data;
  } catch (error) {
    console.error('❌ Erro ao buscar distância:', error);
    throw error;
  }
}

/**
 * Liga a lâmpada manualmente
 * Endpoint: POST /on
 */
export async function turnLightOn(): Promise<ApiControlResponse> {
  try {
    const response = await fetch(`${API_CONFIG.BASE_URL}${API_CONFIG.ENDPOINTS.ON}`, {
      method: 'POST',
      mode: 'cors',
    });

    if (!response.ok) {
      throw new Error(`HTTP ${response.status}: ${response.statusText}`);
    }

    const data = await response.json();
    console.log('✅ Lâmpada ligada:', data.message);
    return data;
  } catch (error) {
    console.error('❌ Erro ao ligar lâmpada:', error);
    throw error;
  }
}

/**
 * Desliga a lâmpada manualmente
 * Endpoint: POST /off
 */
export async function turnLightOff(): Promise<ApiControlResponse> {
  try {
    const response = await fetch(`${API_CONFIG.BASE_URL}${API_CONFIG.ENDPOINTS.OFF}`, {
      method: 'POST',
      mode: 'cors',
    });

    if (!response.ok) {
      throw new Error(`HTTP ${response.status}: ${response.statusText}`);
    }

    const data = await response.json();
    console.log('✅ Lâmpada desligada:', data.message);
    return data;
  } catch (error) {
    console.error('❌ Erro ao desligar lâmpada:', error);
    throw error;
  }
}
