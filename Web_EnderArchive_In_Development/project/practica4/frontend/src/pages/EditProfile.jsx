// src/pages/EditProfile.jsx
import React, { useState, useEffect } from "react";
import Header from "../components/layout/Header.jsx";
import "./MyUserPage.css"; // Reutilizamos el MISMO CSS
import { getUsernameFromToken, fetchUserByUsername, updateUserProfile } from "../services/apiService.js";
import { useNavigate } from "react-router-dom";

export default function EditProfile() {
  const navigate = useNavigate();
  const currentUsername = getUsernameFromToken();

  // Estados del formulario
  const [description, setDescription] = useState("");
  const [avatarUrl, setAvatarUrl] = useState("https://i.pravatar.cc/300?img=12"); // Placeholder inicial
  const [selectedFile, setSelectedFile] = useState(null);
  
  const [isLoading, setIsLoading] = useState(true);
  const [isSaving, setIsSaving] = useState(false);

  // Cargar datos actuales al iniciar
  useEffect(() => {
    const loadData = async () => {
        if (!currentUsername) return;
        try {
            const userData = await fetchUserByUsername(currentUsername);
            // Ajusta estos campos según lo que devuelva tu API (userData.description, etc.)
            if (userData.description) setDescription(userData.description);
            if (userData.profilePic) setAvatarUrl(userData.profilePic); // O la URL completa
        } catch (e) {
            console.error(e);
        } finally {
            setIsLoading(false);
        }
    };
    loadData();
  }, [currentUsername]);

  // Manejo de selección de imagen
  const handleImageChange = (e) => {
    const file = e.target.files[0];
    if (file) {
      setSelectedFile(file);
      // Crear URL temporal para previsualizar
      setAvatarUrl(URL.createObjectURL(file));
    }
  };

  // Guardar cambios
  const handleSave = async () => {
    setIsSaving(true);
    try {
        await updateUserProfile(currentUsername, { description }, selectedFile);
        alert("Perfil actualizado correctamente");
        navigate("/my-profile"); // Volver al perfil
    } catch (e) {
        alert("Error al actualizar: " + e.message);
    } finally {
        setIsSaving(false);
    }
  };

  if (isLoading) return <div className="userpage"><Header /><p style={{padding:"20px", color:"#fff"}}>Cargando...</p></div>;

  return (
    <div className="userpage">
      <Header />

      <div className="top-panel">
        
        {/* Zona perfil */}
        <div className="profile-box">
          
          {/* Avatar con Overlay para editar */}
          <div className="avatar-wrapper">
             <label htmlFor="avatar-upload" className="avatar-edit-label">
                <img src={avatarUrl} alt="Avatar" className="avatar-img" />
                <div className="avatar-overlay">
                    <span>📷 Cambiar</span>
                </div>
             </label>
             <input 
                id="avatar-upload" 
                type="file" 
                accept="image/*" 
                onChange={handleImageChange} 
                style={{display: "none"}} 
             />
          </div>

          {/* Nombre (Solo lectura normalmente, o input si permites cambiarlo) */}
          <div className="identity">
            {/* Si permites cambiar nombre, usa un <input> aquí. Si no, déjalo como h1 pero gris */}
            <h1 className="username" style={{opacity: 0.7}}>{currentUsername}</h1>
            <span style={{fontSize: "12px", color: "#666"}}>(El nombre no se puede cambiar)</span>
          </div>
        </div>

        {/* Zona "Acerca de mí" EDITABLE */}
        <div className="about-box">
            <div className="about-title">Editar Acerca de mí:</div>
            <textarea 
                className="about-textarea editable-field" 
                placeholder="Escribe algo sobre ti..." 
                value={description}
                onChange={(e) => setDescription(e.target.value)}
            />
        </div>
      </div>

      {/* Botones de Acción (Guardar / Cancelar) */}
      <div className="edit-actions" style={{ padding: "0 32px", marginTop: "20px", display:"flex", gap:"10px" }}>
         <button className="edit-btn save-btn" onClick={handleSave} disabled={isSaving}>
            {isSaving ? "Guardando..." : "Guardar Cambios"}
         </button>
         <button className="edit-btn cancel-btn" onClick={() => navigate("/my-profile")}>
            Cancelar
         </button>
      </div>

    </div>
  );
}