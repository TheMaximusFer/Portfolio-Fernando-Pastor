import React, { useEffect, useRef, useState } from "react";
import "./SanctionWindow.css";
import { postBan } from "../services/apiService.js";

export default function SanctionWindow({
  isOpen,
  username = "USUARIO",
  onClose,
  onConfirm, 
}) {
  const [reason, setReason] = useState("");
  const [forever, setForever] = useState(false);
  const [days, setDays] = useState(0);
  const [hours, setHours] = useState(0);
  
  const [isSubmitting, setIsSubmitting] = useState(false);
  const cardRef = useRef(null);

  useEffect(() => {
    if (isOpen) {
        setReason("");
        setForever(false);
        setDays(0);
        setHours(0);
        setIsSubmitting(false);
    }
  }, [isOpen]);

  // Cerrar con ESC
  useEffect(() => {
    if (!isOpen) return;
    const onKey = (e) => e.key === "Escape" && !isSubmitting && onClose?.();
    document.addEventListener("keydown", onKey);
    return () => document.removeEventListener("keydown", onKey);
  }, [isOpen, onClose, isSubmitting]);

  if (!isOpen) return null;

  const clickBackdrop = (e) => {
    if (!isSubmitting && cardRef.current && !cardRef.current.contains(e.target)) {
        onClose?.();
    }
  };

  const handleSanction = async () => {
    if (!reason.trim()) {
        alert("Escribe un motivo.");
        return;
    }

    setIsSubmitting(true);

    const banData = {
      reason: reason.trim(),
      expulsionTotal: forever,
      days: forever ? 0 : Math.max(0, parseInt(days || 0, 10)),
      hours: forever ? 0 : Math.max(0, parseInt(hours || 0, 10)),
    };

    try {
      await postBan(username, banData);
      
      alert(`Usuario ${username} sancionado.`);
      if (onConfirm) onConfirm(); 
      onClose(); 

    } catch (error) {
      console.error(error);
      alert("Error al sancionar: " + error.message);
    } finally {
      setIsSubmitting(false);
    }
  };

  return (
    <div className="sanction-backdrop" onMouseDown={clickBackdrop}>
      <div className="sanction-card" ref={cardRef} onMouseDown={(e)=>e.stopPropagation()}>
        <button className="sanction-close" onClick={!isSubmitting ? onClose : undefined} />

        <h2 className="sanction-title">
          ¿Sancionar a <span className="u">{username}</span>?
        </h2>

        <div className="sanction-section">
          <label className="sanction-label">Motivo:</label>
          <textarea
            className="sanction-textarea"
            placeholder="Motivo..."
            value={reason}
            disabled={isSubmitting}
            onChange={(e) => setReason(e.target.value)}
          />
        </div>

        <div className="sanction-section">
          <div className="sanction-label">Castigo:</div>
          <div className="punish-box">
            <label className="switch">
              <input
                type="checkbox"
                checked={forever}
                disabled={isSubmitting}
                onChange={(e) => setForever(e.target.checked)}
              />
              <span className="slider" />
              <span className="switch-text">Expulsión total</span>
            </label>
          </div>
        </div>

        <div className="sanction-actions">
          <button className="btn btn-ghost" onClick={onClose} disabled={isSubmitting}>Cancelar</button>
          <button 
            className="btn btn-primary" 
            onClick={handleSanction} 
            disabled={isSubmitting || !forever}
          >
            {isSubmitting ? "Enviando..." : "Sancionar"}
          </button>
        </div>
      </div>
    </div>
  );
}