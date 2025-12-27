import java.util.Optional;

import VO_DAO.*;


final public class DAO_VO_test {
    public static void main(String[] args) {
        UserVO usuario1 = new UserVO("paco_garcia", "12321", "Soy paco", false, false, false, null, null, null);
        UserVO usuario2 = new UserVO("maria_gonzalez", "12321", "Soy maria", true, false, false, null, null, null);
        JdbcUserDao dao = new JdbcUserDao();
        
        //Comprobamos si los usuarios existen para eliminarlos si es así y así poder crearlos sin fallos
        Optional<UserVO> u = dao.findUser(usuario1.getUsername());
        if(!u.isEmpty()) {
        	dao.deleteUser(usuario1.getUsername());
        }
        u = dao.findUser(usuario2.getUsername());
        if(!u.isEmpty()) {
        	dao.deleteUser(usuario2.getUsername());
        }
        
        dao.createUser(usuario1);
        dao.createUser(usuario2);
        
        if(dao.findUser(usuario2.getUsername()).isEmpty()) {
            System.out.println("Error al crear usuario dato.");
        }
        UserVO usuario3 = dao.findUser("paco_garcia").get();
        
        Optional<UserVO> usuario4 = dao.findUser("maria_gonzalez");
        if(usuario4.isEmpty()) {
        	System.out.println("Error al buscar el usuario4");
        }
        usuario1.setBanned(true);
        dao.updateUser(usuario1.getUsername(), usuario1);
        usuario3 = dao.findUser(usuario1.getUsername()).get();
        if(!usuario3.getBanned()) {
            System.out.println("Error al actualizar atributo de usuario.");
        }
        usuario4 = dao.findUser("francisco_garcia");
        if(usuario4.isEmpty()) {
        	usuario1.setUsername("francisco_garcia");
	        dao.updateUser(usuario3.getUsername(), usuario1);
	        usuario4 = dao.findUser("francisco_garcia");
	        if(usuario4.isEmpty()) {
	        	System.out.println("Error al actualizar el nombre de usuario");
	        }
        }else {
        	dao.deleteUser(usuario4.get().getUsername());
        	usuario1.setUsername("francisco_garcia");
	        dao.updateUser(usuario3.getUsername(), usuario1);
	        usuario4 = dao.findUser("francisco_garcia");
	        if(usuario4.isEmpty()) {
	        	System.out.println("Error al actualizar el nombre de usuario");
	        }
        }
        
        
        
        dao.deleteUser(usuario2.getUsername());
        if(!dao.findUser(usuario2.getUsername()).isEmpty()) {
            System.out.println("Error al eliminar usuario dato.");
        }
    }
}