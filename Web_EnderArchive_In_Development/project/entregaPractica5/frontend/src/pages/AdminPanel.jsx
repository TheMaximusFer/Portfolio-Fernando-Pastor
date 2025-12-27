import React, { useState, useEffect } from "react";
import Header from "../components/layout/Header.jsx";
import NavigationBar from "../components/layout/NavigationBar.jsx"; 
import SanctionWindow from "./SanctionWindow.jsx";
import "./AdminPanel.css";
import { 
    fetchReports, 
    fetchUsers, 
} from "../services/apiService.js";
import { useNavigate } from "react-router-dom";
import { ResponsiveContainer, PieChart, Pie, Cell, BarChart, Bar, XAxis, YAxis, Tooltip } from "recharts";

export default function AdminPanel() {
  const navigate = useNavigate();

  const [reports, setReports] = useState([]);
  const [users, setUsers] = useState([]);
  const [loading, setLoading] = useState(true);

  // ESTADOS DEL MODAL
  const [showSanctionModal, setShowSanctionModal] = useState(false);
  const [userToBan, setUserToBan] = useState("");
  const [quickSanctionUser, setQuickSanctionUser] = useState("");

  const pieData = [{ name: "Free", value: 40 }, { name: "Pro", value: 10 }, { name: "Activos", value: 30 }, { name: "Otros", value: 20 }];
  const PIE_COLORS = ["#28c3d9", "#f0a21b", "#5f79ff", "#82a035"];
  const barsData = [{ name: "L", val: 160 }, { name: "M", val: 100 }, { name: "X", val: 140 }, { name: "J", val: 90 }, { name: "V", val: 150 }];

  useEffect(() => {
    loadAdminData();
  }, []);

  const loadAdminData = async () => {
    setLoading(true);
    try {
        const [reportsData, usersData] = await Promise.all([
            fetchReports().catch(e => []),
            fetchUsers().catch(e => [])
        ]);
        
        const mappedReports = Array.isArray(reportsData) ? reportsData.map(r => ({
            id: r.id, 
            reporter: r.id?.reporterUsername || "Anónimo",
            reported: r.id?.reportedUsername || "Desconocido",
            rawUrl: r.urlContext,
            worldDisplay: r.urlContext,
            description: r.reportDescription || "Sin descripción"
        })) : [];

        setReports(mappedReports);
        setUsers(Array.isArray(usersData) ? usersData : []);
    } catch (error) {
        console.error("Error cargando panel:", error);
    } finally {
        setLoading(false);
    }
  };

  const handleOpenSanction = (username) => {
    if(!username) return;
    setUserToBan(username);
    setShowSanctionModal(true);
  };

  const onSanctionComplete = () => {
    loadAdminData();
  };

  return (
    <div className="adminpage">
      <Header />
      <NavigationBar />

      <div className="admin-shell">
        <h1 className="admin-h1">PANEL ADMIN</h1>

        <div className="admin-grid">
          {/* 1. ESTADÍSTICAS */}
          <section className="admin-card">
            <div className="card-head">ESTADÍSTICAS</div>
            <div className="stats-body">
              <ul className="stats-list">
                <li>Usuarios: <b>{users.length}</b></li>
                <li>Reportes: <b>{reports.length}</b></li>
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

          {/* 2. USUARIOS */}
          <section className="admin-card">
            <div className="card-head">USUARIOS</div>
            <div className="logs-box">
              {users.map((u, i) => (
                <div key={i} className="log-line" style={{display:'flex', justifyContent:'space-between'}}>
                    <span>{u.username}</span>
                    <button 
                        style={{padding:'2px 8px', fontSize:'0.75rem', background:'#ff4757', color:'white', border:'none', borderRadius:'4px', cursor:'pointer'}}
                        onClick={() => handleOpenSanction(u.username)}
                    >
                        BAN
                    </button>
                </div>
              ))}
            </div>
            
            <div className="quick-sanction">
              <label>Sanción rápida:</label>
              <div className="qs-row">
                <input 
                    className="qs-input" 
                    placeholder="Usuario..." 
                    value={quickSanctionUser} 
                    onChange={(e) => setQuickSanctionUser(e.target.value)} 
                />
                <button className="btn-primary" onClick={() => handleOpenSanction(quickSanctionUser)}>
                    Sancionar
                </button>
              </div>
            </div>
          </section>

          {/* 3. DENUNCIAS */}
          <aside className="admin-card">
            <div className="card-head">DENUNCIAS ({reports.length})</div>
            <div className="reports-list">
              {reports.length === 0 && <p style={{textAlign:"center", padding:"20px", color:"#aaa"}}>No hay denuncias.</p>}
              
              {reports.map((r, idx) => (
                <article key={idx} className="report-item">
                  <button className="close-x" title="Descartar" onClick={() => {/* Lógica borrar */}} />
                  
                  <div className="report-lines">
                    <div className="report-row">
                        <span className="lbl">DE:</span> <span className="val">{r.reporter}</span>
                    </div>
                    <div className="report-row">
                        <span className="lbl">A:</span> <span className="val reported">{r.reported}</span>
                    </div>
                    {r.worldDisplay && (
                        <div className="report-row">
                            <span className="lbl">SITIO:</span> <span className="val" style={{fontSize:'0.8em', color:'#aaa'}}>{r.worldDisplay}</span>
                        </div>
                    )}
                    <div className="report-desc-box">"{r.description}"</div>
                  </div>

                  <div className="report-cta">
                    <button 
                        className="btn-context" 
                        onClick={() => navigate(r.rawUrl)}
                    >
                        Ir a su contexto
                    </button>
                    <button 
                        className="btn-context" 
                        onClick={() => handleOpenSanction(r.reported)}
                    >
                        Sancionar
                    </button>
                  </div>
                </article>
              ))}
            </div>
          </aside>
        </div>
      </div>

      <SanctionWindow 
          isOpen={showSanctionModal}
          username={userToBan} 
          onClose={() => setShowSanctionModal(false)}
          onConfirm={onSanctionComplete}
      />
    </div>
  );
}