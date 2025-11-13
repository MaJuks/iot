import { useState, useEffect } from 'react';
import { turnLightOn, turnLightOff, fetchStatus, fetchLogs } from './services/arduinoApi';
import { API_CONFIG } from './config/api';
import type { ApiStatus, ApiLog } from './config/api';
import './App.css';

function App() {
  const [status, setStatus] = useState<ApiStatus | null>(null);
  const [logs, setLogs] = useState<ApiLog[]>([]);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);

  const loadData = async () => {
    try {
      setError(null);
      const [statusData, logsData] = await Promise.all([
        fetchStatus(),
        fetchLogs()
      ]);
      setStatus(statusData);
      setLogs(logsData.logs || []);
    } catch (err) {
      setError('Erro ao conectar com Arduino');
      console.error(err);
    }
  };

  useEffect(() => {
    loadData();
    const interval = setInterval(loadData, API_CONFIG.REFRESH_INTERVAL);
    return () => clearInterval(interval);
  }, []);

  const handleTurnOn = async () => {
    setLoading(true);
    try {
      await turnLightOn();
      setTimeout(() => {
        loadData();
        setLoading(false);
      }, 500);
    } catch (err) {
      setError('Erro ao ligar');
      setLoading(false);
    }
  };

  const handleTurnOff = async () => {
    setLoading(true);
    try {
      await turnLightOff();
      setTimeout(() => {
        loadData();
        setLoading(false);
      }, 500);
    } catch (err) {
      setError('Erro ao desligar');
      setLoading(false);
    }
  };

  const formatTime = (timestamp: number) => {
    const date = new Date(timestamp);
    return date.toLocaleTimeString('pt-BR', { hour: '2-digit', minute: '2-digit' });
  };

  return (
    <div className="app">
      <div className="container">
        <h1>Controle de Lâmpada</h1>

        {error && <div className="error">{error}</div>}

        {/* Status */}
        <div className="card">
          <h2>Status</h2>
          <div className="status">
            <div className={`indicator ${status?.isOn ? 'on' : 'off'}`}></div>
            <span>{status?.isOn ? 'Ligada' : 'Desligada'}</span>
          </div>
        </div>

        {/* Controles */}
        <div className="card">
          <h2>Controle</h2>
          <div className="controls">
            <button
              onClick={handleTurnOn}
              disabled={loading || status?.isOn}
              className="btn-on"
            >
              Ligar
            </button>
            <button
              onClick={handleTurnOff}
              disabled={loading || !status?.isOn}
              className="btn-off"
            >
              Desligar
            </button>
          </div>
        </div>

        {/* Logs */}
        <div className="card">
          <h2>Histórico</h2>
          <div className="logs">
            {logs.length === 0 ? (
              <p className="empty">Nenhum registro</p>
            ) : (
              logs.slice(0, 10).map((log) => (
                <div key={log.id} className="log-item">
                  <div className={`log-dot ${log.type}`}></div>
                  <div className="log-info">
                    <span className="log-type">
                      {log.type === 'on' ? 'Ligada' : 'Desligada'}
                    </span>
                    <span className="log-time">{formatTime(log.timestamp)}</span>
                  </div>
                  {log.duration > 0 && (
                    <span className="log-duration">{log.duration}s</span>
                  )}
                </div>
              ))
            )}
          </div>
        </div>
      </div>
    </div>
  );
}

export default App;
