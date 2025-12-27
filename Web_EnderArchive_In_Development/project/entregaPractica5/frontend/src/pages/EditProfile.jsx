// src/pages/EditProfile.jsx
import React, { useState, useEffect } from "react";
import Header from "../components/layout/Header.jsx";
import "./MyUserPage.css"; 
import "./EditProfile.css"; // <--- NUEVO ARCHIVO CSS IMPORTADO
import { getUsernameFromToken, fetchUserByUsername, updateUserProfile } from "../services/apiService.js";
import { useNavigate } from "react-router-dom";

export default function EditProfile() {
  const navigate = useNavigate();
  const currentUsername = getUsernameFromToken();

  const [description, setDescription] = useState("");
  const [email, setEmail] = useState("");
  const [admin, setAdmin] = useState(false);
  const [password, setPassword] = useState(""); 
  const [confirmPassword, setConfirmPassword] = useState("");
  const [avatarUrl, setAvatarUrl] = useState("https://i.pravatar.cc/300?img=12");
  const [selectedFile, setSelectedFile] = useState(null);
  const [message, setMessage] = useState('');
  const [isLoading, setIsLoading] = useState(true);
  const [isSaving, setIsSaving] = useState(false);

  useEffect(() => {
    const loadData = async () => {
        if (!currentUsername) return;
        try {
            const userData = await fetchUserByUsername(currentUsername);
            if (userData.description) setDescription(userData.description);
            if (userData.email) setEmail(userData.email);
            if (userData.admin) setAdmin(userData.admin);
            if (userData.profilePic) setAvatarUrl(userData.profilePic);
        } catch (e) {
            console.error(e);
        } finally {
            setIsLoading(false);
        }
    };
    loadData();
  }, [currentUsername]);

  const handleImageChange = (e) => {
    const file = e.target.files[0];
    if (file) {
      setSelectedFile(file);
      setAvatarUrl(URL.createObjectURL(file));
    }
  };

  const handleSave = async () => {
    setIsSaving(true);
    setMessage(""); // Limpiar errores previos
    try {
      var updatedPassword = ""
      if (password !== "") {
        if(password.length < 6 || password.length > 50) {
          setMessage("La contraseña debe tener entre 6 y 50 caracteres");
          setIsSaving(false);
          return;
        } else if (password !== confirmPassword) {
          setMessage("Las contraseñas no coinciden");
          setIsSaving(false);
          return;
        }
        updatedPassword = { password: password };
      }
      
      const userDTO = {
        description: description,
        admin: admin,
        email: email,
        ...updatedPassword,
      }; 
      
      console.log(userDTO.description)
      await updateUserProfile(userDTO, selectedFile);
      alert("Perfil actualizado correctamente");
      navigate("/my-profile");
    } catch (e) {
        alert("Error al actualizar: " + e.message);
    } finally {
        setIsSaving(false);
    }
  };

  if (isLoading) return <div className="userpage"><Header /><div className="loading-text">Cargando perfil...</div></div>;

  return (
    <div className="userpage edit-mode">
      <Header />

      <div className="top-panel">
        
        {/* COLUMNA IZQUIERDA: Avatar y Nombre */}
        <div className="profile-box fixed-height">
          <div className="avatar-wrapper">
             <label htmlFor="avatar-upload" className="avatar-edit-label">
                <img src={avatarUrl} alt="Avatar" className="avatar-img" />
                <div className="avatar-overlay">
                    <span className="camera-icon">📷</span>
                    <span>Cambiar</span>
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

          <div className="identity">
            <h1 className="username disabled-text">{currentUsername}</h1>
            <span className="info-subtext">El nombre de usuario no se puede cambiar</span>
          </div>
        </div>

        {/* COLUMNA DERECHA: Campos de Edición (Agrupados para que el Grid no se rompa) */}
        <div className="edit-column">
            
            {/* Acerca de mí */}
            <div className="edit-card">
                <label className="edit-label">Acerca de mí</label>
                <textarea 
                    className="edit-input textarea-mode" 
                    placeholder="Cuéntanos algo sobre ti..." 
                    value={description}
                    onChange={(e) => setDescription(e.target.value)}
                    rows={4}
                />
            </div>

            {/* Correo */}
            <div className="edit-card">
                <label className="edit-label">Correo Electrónico</label>
                <input 
                    type="email"
                    className="edit-input" 
                    placeholder="tu@email.com" 
                    value={email}
                    onChange={(e) => setEmail(e.target.value)}
                />
            </div>

            {/* Contraseñas (Usamos inputs type password por seguridad) */}
            <div className="password-grid">
                <div className="edit-card">
                    <label className="edit-label">Nueva contraseña</label>
                    <input 
                        type="password"
                        className="edit-input" 
                        placeholder="••••••" 
                        value={password}
                        onChange={(e) => setPassword(e.target.value)}
                    />
                </div>
                <div className="edit-card">
                    <label className="edit-label">Confirmar contraseña</label>
                    <input 
                        type="password"
                        className="edit-input" 
                        placeholder="••••••" 
                        value={confirmPassword}
                        onChange={(e) => setConfirmPassword(e.target.value)}
                    />
                </div>
            </div>

            {/* Mensajes de error */}
            {message && <div className="error-msg">{message}</div>}

            {/* Botones */}
            <div className="edit-actions">
                <button className="btn-action cancel" onClick={() => navigate("/my-profile")}>
                    Cancelar
                </button>
                <button className="btn-action save" onClick={handleSave} disabled={isSaving}>
                    {isSaving ? "Guardando..." : "Guardar Cambios"}
                </button>
            </div>

        </div>
      </div>
    </div>
  );
}