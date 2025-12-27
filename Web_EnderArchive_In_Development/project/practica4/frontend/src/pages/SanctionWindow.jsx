import React, { useEffect, useRef, useState } from "react";
import "./SanctionWindow.css";

export default function SanctionWindow({
  isOpen,
  username = "NOMBRE_USUARIO",
  onClose,
  onConfirm,
}) {
  const [reason, setReason] = useState("");
  const [forever, setForever] = useState(false);
  const [days, setDays] = useState(0);
  const [hours, setHours] = useState(0);

  const cardRef = useRef(null);

  // Cerrar con ESC y bloquear scroll fondo
  useEffect(() => {
    if (!isOpen) return;
    const onKey = (e) => e.key === "Escape" && onClose?.();
    document.addEventListener("keydown", onKey);
    const prev = document.body.style.overflow;
    document.body.style.overflow = "hidden";
    return () => {
      document.removeEventListener("keydown", onKey);
      document.body.style.overflow = prev;
    };
  }, [isOpen, onClose]);

  if (!isOpen) return null;

  const clickBackdrop = (e) => {
    if (!cardRef.current) return;
    if (!cardRef.current.contains(e.target)) onClose?.();
  };

  const commit = () => {
    const payload = {
      username,
      reason: reason.trim(),
      expulsionTotal: forever,
      days: forever ? 0 : Math.max(0, parseInt(days || 0, 10)),
      hours: forever ? 0 : Math.max(0, parseInt(hours || 0, 10)),
    };
    onConfirm?.(payload);
  };

  return (
    <div className="sanction-backdrop" onMouseDown={clickBackdrop}>
      <div className="sanction-card" ref={cardRef}>
        {/* Cerrar (X) */}
        <button className="sanction-close" aria-label="Cerrar" onClick={onClose} />

        {/* Título */}
        <h2 className="sanction-title">
          ¿Sancionar a <span className="u">{username}</span>?
        </h2>

        {/* Motivo */}
        <div className="sanction-section">
          <label className="sanction-label" htmlFor="motivo">Motivo:</label>
          <textarea
            id="motivo"
            className="sanction-textarea"
            placeholder="Describe el motivo de la sanción…"
            value={reason}
            onChange={(e) => setReason(e.target.value)}
          />
        </div>

        {/* Castigo */}
        <div className="sanction-section">
          <div className="sanction-label">Castigo:</div>

          <div className="punish-box">
            <label className="switch">
              <input
                type="checkbox"
                checked={forever}
                onChange={(e) => setForever(e.target.checked)}
              />
              <span className="slider" />
              <span className="switch-text">Expulsión total</span>
            </label>

            <div className="time-fields">
              <span>Expulsar por&nbsp;</span>
              <input
                type="number"
                min="0"
                className="num"
                value={days}
                onChange={(e) => setDays(e.target.value)}
                disabled={forever}
              />
              <span>&nbsp;días&nbsp;</span>
              <input
                type="number"
                min="0"
                className="num"
                value={hours}
                onChange={(e) => setHours(e.target.value)}
                disabled={forever}
              />
              <span>&nbsp;horas</span>
            </div>
          </div>
        </div>

        {/* Botones */}
        <div className="sanction-actions">
          <button className="btn btn-ghost" onClick={onClose}>Cancelar</button>
          <button className="btn btn-primary" onClick={commit}>Sancionar</button>
        </div>
      </div>
    </div>
  );
}
