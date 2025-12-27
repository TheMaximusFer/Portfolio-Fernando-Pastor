import React from 'react';
import { BrowserRouter, Routes, Route } from 'react-router-dom';

// Importa tus componentes de página
import HomePage from './pages/HomePage';
import RegisterAccount from './pages/RegisterAccount';
import LoginScreen from './pages/LoginScreen';
import MyUserPage from './pages/MyUserPage';
import UserPage from './pages/UserPage'
import WorldDetail from './pages/WorldDetail';
import WorldCreate from './pages/WorldCreate';
import AdminPanel from './pages/AdminPanel';
import EditProfile from './pages/EditProfile';
import Particles from './components/background/Particles';


function App() {

return ( 
    <React.StrictMode>
      <BrowserRouter>
        
        {/* --- INICIO DEL FONDO --- */}
        {/* Usamos position: fixed para que se quede pegado a la pantalla 
            y zIndex: -1 para que se vaya al fondo detrás del contenido */}
        <div style={{ 
          position: 'fixed', 
          top: 0, 
          left: 0, 
          width: '100%', 
          height: '100%', 
          zIndex: -1,
          backgroundColor: '#0b0514' /* Color de fondo base por si acaso */
        }}>
          <Particles
            particleColors={['#9219F5', '#9219F5']}
            particleCount={700}
            particleSpread={10}
            speed={0.04}
            particleBaseSize={100}
            moveParticlesOnHover={true}
            alphaParticles={false}
            disableRotation={false}
          />
        </div>
        {/* --- FIN DEL FONDO --- */}

        <Routes>
          <Route path="/" element={<HomePage />} /> 
          <Route path="/register" element={<RegisterAccount />} />
          <Route path="/login" element={<LoginScreen />} />
          <Route path="/my-profile" element={<MyUserPage />}/>
          <Route path="/user-profile/:username" element={<UserPage />}/>
          <Route path="/user-profile/:worldOwner/:worldName" element={<WorldDetail />}/>
          <Route path="/create-world" element={<WorldCreate />} />
          <Route path="/admin" element={<AdminPanel />} />
          <Route path="/edit-profile" element={<EditProfile />} />
        </Routes>

      </BrowserRouter>
    </React.StrictMode>
  );
}

export default App
