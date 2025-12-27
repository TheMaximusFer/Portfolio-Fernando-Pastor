import React, { useEffect, useRef, useState } from "react";
import { useLocation } from "react-router-dom";
import "./ReportWindow.css";
// 1. IMPORTAMOS LAS FUNCIONES DE LA API
import { postReport, getUsernameFromToken } from "../services/apiService.js";

export default function ReportWindow({
  isOpen,
  username = "NOMBRE_USUARIO", // El usuario al que estamos denunciando
  onClose,
  onConfirm,
  primaryLabel = "Denunciar",
}) {
  const [reason, setReason] = useState("");
  const [isSubmitting, setIsSubmitting] = useState(false); // Estado para evitar doble clic
  const cardRef = useRef(null);

  const location = useLocation();

  // Cerrar con Esc
  useEffect(() => {
    if (!isOpen) return;
    const onKey = (e) => e.key === "Escape" && !isSubmitting && onClose?.();
    window.addEventListener("keydown", onKey);
    return () => window.removeEventListener("keydown", onKey);
  }, [isOpen, onClose, isSubmitting]);

  if (!isOpen) return null;

  // 2. FUNCIÓN PARA ENVIAR A LA BASE DE DATOS
  const submit = async () => {
    // 1. Obtenemos el usuario del token solo para validar que está logueado
    const currentUser = getUsernameFromToken();
    
    if (!currentUser) {
      alert("Debes iniciar sesión para poder denunciar.");
      return;
    }

    if (!reason.trim()) return;

    setIsSubmitting(true);

    // 2. CAMBIO: Construimos el JSON exactamente como pide el Backend
    // (Sin 'reporterUsername', ya que el Backend lo saca de tu Token)
    const reportData = {
      reportedUsername: username,      // A quién denuncias
      reportDescription: reason.trim(),// Por qué
      urlContext: location.pathname    // Dónde
    };

    try {
      await postReport(reportData);
      
      alert("Denuncia enviada correctamente.");
      setReason(""); 
      onConfirm?.(); 
      onClose();     
      
    } catch (error) {
      console.error("Error enviando reporte:", error);
      // Ahora verás el mensaje real del backend si falla
      alert(error.message || "Hubo un error al enviar la denuncia.");
    } finally {
      setIsSubmitting(false);
    }
  };

  const clickBackdrop = (e) => {
    // Cierra si clicas fuera de la tarjeta y no se está enviando
    if (!isSubmitting && cardRef.current && !cardRef.current.contains(e.target)) {
      onClose?.();
    }
  };

  return (
    <div className="report-backdrop" onMouseDown={clickBackdrop} role="dialog" aria-modal="true">
      <div className="report-card" ref={cardRef} onMouseDown={(e) => e.stopPropagation()}>
        
        {/* Botón X deshabilitado si está enviando */}
        <button 
            className="report-close" 
            aria-label="Cerrar" 
            onClick={onClose} 
            disabled={isSubmitting}
        />

        <h2 className="report-title">
          ¿Denunciar a <span className="u">{username}</span>?
        </h2>

        <div className="report-section">
          <label className="report-label" htmlFor="report-reason">Motivo:</label>
          <textarea
            id="report-reason"
            className="report-textarea"
            placeholder="Describe el motivo de la denuncia…"
            value={reason}
            disabled={isSubmitting}
            onChange={(e) => setReason(e.target.value)}
          />
        </div>

        <div className="report-actions">
          <button 
            className="btn btn-ghost" 
            onClick={onClose} 
            disabled={isSubmitting}
          >
            Cancelar
          </button>
          
          <button
            className="btn btn-primaryy"
            onClick={submit}
            disabled={!reason.trim() || isSubmitting}
          >
            {isSubmitting ? "Enviando..." : primaryLabel}
          </button>
        </div>
      </div>
    </div>
  );
}