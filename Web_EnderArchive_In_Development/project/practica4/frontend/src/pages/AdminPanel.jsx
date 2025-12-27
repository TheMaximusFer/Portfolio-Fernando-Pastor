import React, { useState, useEffect } from "react";
import Header from "../components/layout/Header.jsx";
import SanctionWindow from "./SanctionWindow.jsx";
import "./AdminPanel.css";
//import { fetchReports, deleteReport } from "../services/apiService.js";
import { useNavigate } from "react-router-dom"; // Importamos para el botón de contexto
import { fetchReports, deleteReport } from "../services/apiService.js";
// Recharts
import {
  ResponsiveContainer, PieChart, Pie, Cell, BarChart, Bar, XAxis, YAxis, Tooltip,
} from "recharts";

export default function AdminPanel() {
  const navigate = useNavigate(); // Hook para navegar

  // Datos Mocks
  const stats = { users: 120, active: 15, newThisMonth: 12, bannedThisMonth: 2, revenueThisMonth: 0 };
  const [logs] = useState(["---EN OBRAS---", "PROXIMAMENTE..."]);
  const [reports, setReports] = useState([]);
  const [isLoading, setIsLoading] = useState(true);

  // Sanción
  const [openSanction, setOpenSanction] = useState(false);
  const [sanctionUser, setSanctionUser] = useState("");

  useEffect(() => {
    loadReports();
  }, []);

// Dentro de AdminPanel.jsx

  // Dentro de AdminPanel.jsx

const loadReports = async () => {
  try {
    setIsLoading(true);
    const data = await fetchReports();
    
    // IMPORTANTE: Mira la consola del navegador.
    // Aquí verás exactamente qué propiedad está enviando tu backend.
    console.log("DATOS RECIBIDOS DEL BACKEND:", data); 

    const findName = (item, keys) => {
        // Si el item es null, salimos
        if (!item) return null;
        
        for (const key of keys) {
            const val = item[key];
            if (!val) continue;
            // Si es objeto (ej: user: {username: "x"}), devolvemos username
            if (typeof val === 'object' && val.username) return val.username;
            // Si es string directo, lo devolvemos
            if (typeof val === 'string') return val;
        }
        return null;
    };

    const mappedReports = Array.isArray(data) ? data.map(r => ({
      id: r.id, // Asegúrate de que tu Backend devuelve un campo 'id'
      
      // Ampliamos las claves de búsqueda por si acaso
      reporter: r.id.reporterUsername,
      
      reported: r.id.reportedUsername,
      
      rawUrl: r.urlContext,
      worldDisplay: r.urlContext,
      description: r.reportDescription || "Sin descripción"
    })) : [];

    setReports(mappedReports);
  } catch (error) {
    console.error("Error cargando denuncias:", error);
    // Opcional: mostrar error visual
  } finally {
    setIsLoading(false);
  }
};

const removeReport = async (report) => {
    if(!window.confirm(`¿Borrar denuncia de ${report.reporter} a ${report.reported}?`)) return;

    try {
      // 2. Llamamos a la API con los dos nombres
      await deleteReport(report.reporter, report.reported);
      
      // 3. Actualizamos la lista visual (seguimos usando ID para filtrar localmente si es único)
      setReports((prev) => prev.filter((r) => r.id !== report.id));
      
    } catch (error) {
      alert("Error al eliminar: " + error.message);
    }
  };

  // Datos Gráficas
  const pieData = [{ name: "Free", value: 40 }, { name: "Pro", value: 10 }, { name: "Activos", value: 30 }, { name: "Otros", value: 20 }];
  const PIE_COLORS = ["#28c3d9", "#f0a21b", "#5f79ff", "#82a035"];
  const barsData = [{ name: "L", val: 160 }, { name: "M", val: 100 }, { name: "X", val: 140 }, { name: "J", val: 90 }, { name: "V", val: 150 }];

  return (
    <div className="adminpage">
      <Header />

      <div className="admin-shell">
        <h1 className="admin-h1">PANEL ADMIN</h1>

        <div className="admin-grid">
          {/* ======= ESTADÍSTICAS ======= */}
          <section className="admin-card">
            <div className="card-head">ESTADÍSTICAS:</div>
            <div className="stats-body">
              <ul className="stats-list">
                <li><b>Número de usuarios:</b> {stats.users}</li>
                <li><b>Usuarios activos ahora:</b> {stats.active}</li>
                <li><b>Nuevos usuarios este mes:</b> {stats.newThisMonth}</li>
                <li><b>Baneados este mes:</b> {stats.bannedThisMonth}</li>
                <li><b>Dinero recaudado:</b> {stats.revenueThisMonth}€</li>
              </ul>
              <div className="charts">
                <div className="chart-wrap">
                  <ResponsiveContainer width="100%" height="100%">
                    <PieChart>
                      <Pie data={pieData} dataKey="value" outerRadius={80} stroke="#1f1f26">
                        {pieData.map((e, i) => <Cell key={i} fill={PIE_COLORS[i % PIE_COLORS.length]} />)}
                      </Pie>
                      <Tooltip />
                    </PieChart>
                  </ResponsiveContainer>
                </div>
                <div className="chart-bars-wrap">
                  <ResponsiveContainer width="100%" height="100%">
                    <BarChart data={barsData}>
                      <XAxis dataKey="name" stroke="#cfcff6" />
                      <YAxis stroke="#cfcff6" />
                      <Tooltip />
                      <Bar dataKey="val" fill="#7a68ff" radius={[6, 6, 0, 0]} />
                    </BarChart>
                  </ResponsiveContainer>
                </div>
              </div>
            </div>
          </section>

          {/* ======= LOGS ======= */}
          <section className="admin-card">
            <div className="card-head">ÚLTIMOS LOGS</div>
            <div className="logs-box">
              {logs.map((l, i) => <div key={i} className="log-line">{l}</div>)}
            </div>
            <div className="quick-sanction">
              <label htmlFor="qs-name">Sanción rápida:</label>
              <div className="qs-row">
                <input id="qs-name" className="qs-input" placeholder="usuario" value={sanctionUser} onChange={(e) => setSanctionUser(e.target.value)} />
                <button className="btn-primary" onClick={() => sanctionUser && setOpenSanction(true)}>Sancionar</button>
              </div>
            </div>
          </section>

          {/* ======= DENUNCIAS ======= */}
          <aside className="admin-card">
            <div className="card-head">DENUNCIAS ({reports.length})</div>
            <div className="reports-list">
              {isLoading && <p style={{textAlign:"center", padding:"10px"}}>Cargando...</p>}
              {!isLoading && reports.length === 0 && <p style={{textAlign:"center", opacity:0.7, padding:"10px"}}>Limpio.</p>}

              {reports.map((r) => (
            <article key={r.id} className="report-item">
              {/* CAMBIO AQUÍ: Pasamos el objeto 'r' entero o los campos específicos */}
              <button 
                className="close-x" 
                onClick={() => removeReport(r)} 
                title="Borrar denuncia" 
              />
                  
                  <div className="report-lines">
                    <div><b>De:</b> {r.reporter}</div>
                    <div><b>A:</b> <span style={{color: "#ff6b6b"}}>{r.reported}</span></div>
                    
                    {r.worldDisplay && r.worldDisplay !== "-" && (
                        <div><b>Contexto:</b> {r.worldDisplay}</div>
                    )}
                    
                    <div style={{marginTop: "5px", fontStyle:"italic", opacity: 0.9}}>"{r.description}"</div>
                  </div>

                  <div className="report-cta">
                    {r.rawUrl && (
                        <button 
                            className="btn-context"
                            onClick={() => navigate(r.rawUrl)}
                        >
                            Ir al lugar
                        </button>
                    )}
                  </div>
                </article>
              ))}
            </div>
          </aside>
        </div>
      </div>

      <SanctionWindow
        isOpen={openSanction}
        username={sanctionUser}
        onClose={() => setOpenSanction(false)}
        onConfirm={(payload) => { console.log(payload); setOpenSanction(false); }}
      />
    </div>
  );
}