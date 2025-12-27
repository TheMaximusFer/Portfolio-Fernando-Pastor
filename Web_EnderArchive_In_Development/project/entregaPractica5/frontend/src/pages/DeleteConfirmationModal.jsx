// src/components/modals/DeleteConfirmationModal.jsx
import React, { useEffect, useRef } from "react";
import "./DeleteConfirmationModal.css";

export default function DeleteConfirmationModal({
  isOpen,
  worldName = "este mundo",
  onClose,
  onConfirm,
  isDeleting = false, // Para mostrar "Eliminando..."
}) {
  const cardRef = useRef(null);

  // Cerrar con tecla Escape
  useEffect(() => {
    if (!isOpen) return;
    const onKey = (e) => e.key === "Escape" && !isDeleting && onClose?.();
    window.addEventListener("keydown", onKey);
    return () => window.removeEventListener("keydown", onKey);
  }, [isOpen, onClose, isDeleting]);

  // Si no está abierto, no renderizamos nada
  if (!isOpen) return null;

  // Cerrar al clicar en el fondo oscuro
  const clickBackdrop = (e) => {
    if (!isDeleting && cardRef.current && !cardRef.current.contains(e.target)) {
      onClose?.();
    }
  };

  return (
    <div 
        className="delete-backdrop" 
        onMouseDown={clickBackdrop} 
        role="dialog" 
        aria-modal="true"
    >
      <div 
        className="delete-card" 
        ref={cardRef} 
        onMouseDown={(e) => e.stopPropagation()}
      >
        {/* Botón X */}
        <button 
            className="delete-close" 
            aria-label="Cerrar" 
            onClick={!isDeleting ? onClose : undefined} 
        />

        {/* Título */}
        <h2 className="delete-title">
          ¿Eliminar <span className="highlight">{worldName}</span>?
        </h2>

        {/* Texto de aviso */}
        <p className="delete-warning">
          Esta acción eliminará el mundo y todos sus archivos de forma permanente. 
          <br />
          <strong>¡No se puede deshacer!</strong>
        </p>

        {/* Botones de acción */}
        <div className="delete-actions">
          <button 
            className="btn-modal btn-cancel" 
            onClick={onClose} 
            disabled={isDeleting}
          >
            Cancelar
          </button>
          
          <button
            className="btn-modal btn-danger"
            onClick={onConfirm}
            disabled={isDeleting}
          >
            {isDeleting ? "Eliminando..." : "Eliminar Mundo"}
          </button>
        </div>
      </div>
    </div>
  );
}