package VO_DAO;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.*;
import java.util.Optional;
import VO_DAO.ConnectionManager;

import javax.sql.DataSource;

public final class JdbcUserDao {

    private static String userCreate = "INSERT INTO \"sisInf\".\"user\" (username, password, description, admin, sanctioned, banned, end_of_sanction, sanction_reason, profile_picture_file_path) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)";
    private static String userSearch = "SELECT username, password, description, admin, sanctioned, banned, end_of_sanction, sanction_reason, profile_picture_file_path FROM \"sisInf\".\"user\" WHERE username = ?";
    private static String userUpdate = "UPDATE \"sisInf\".\"user\" SET username = ?, password = ?, description = ?, admin = ?, sanctioned = ?, banned = ?, end_of_sanction = ?, sanction_reason = ?, profile_picture_file_path = ? WHERE username = ?";
    private static String userDelete = "DELETE FROM \"sisInf\".\"user\" WHERE username = ?";
    

    public Boolean createUser(UserVO user) { 
        
        try {
        	Connection c = ConnectionManager.getConnection(); 
            PreparedStatement ps = c.prepareStatement(userCreate);
                ps.setString(1, user.getUsername());
                ps.setString(2, user.getPassword());
                ps.setString(3, user.getDescription());
                ps.setBoolean(4, user.getAdmin());
                ps.setBoolean(5, user.getSanctioned());
                ps.setBoolean(6, user.getBanned());
                ps.setTimestamp(7, user.getEndOfSanction());
                ps.setString(8, user.getSanction_Reason());
                ps.setString(9, user.getProfile_pic_filepath());
            int rows = ps.executeUpdate();

            ps.close();
            c.close();

            return rows > 0;
            
        } catch (SQLException se) {
            se.printStackTrace();
            return false;
        }   catch(Exception e) {
            e.printStackTrace(System.err);
            return false;
        }

    }

    public Optional<UserVO> findUser(String username) { 
        
        try {
        	Connection c = ConnectionManager.getConnection();
            PreparedStatement ps = c.prepareStatement(userSearch);
            ps.setString(1, username);
            ResultSet rs = ps.executeQuery();
            if (!rs.next()) return Optional.empty();
           
            Optional<UserVO> user = Optional.of(new UserVO(
                rs.getString("username"),
                rs.getString("password"),
                rs.getString("description"),
                rs.getBoolean("admin"),
                rs.getBoolean("sanctioned"),
                rs.getBoolean("banned"),
                rs.getTimestamp("end_of_sanction"),
                rs.getString("sanction_reason"),
                rs.getString("profile_picture_file_path")
            ));

            rs.close();
            ps.close();
            c.close();

            return user;
            
        } catch (SQLException se) {
            se.printStackTrace();
            return Optional.empty();
        }   catch(Exception e) {
            e.printStackTrace(System.err);
            return Optional.empty();
        }

    }

    public Boolean updateUser(String username,UserVO user) { 
        
        try {
        	Connection c = ConnectionManager.getConnection();
        	 PreparedStatement ps = c.prepareStatement(userUpdate);
             ps.setString(1, user.getUsername());
             ps.setString(2, user.getPassword());
             ps.setString(3, user.getDescription());
             ps.setBoolean(4, user.getAdmin());
             ps.setBoolean(5, user.getSanctioned());
             ps.setBoolean(6, user.getBanned());
             ps.setTimestamp(7, user.getEndOfSanction());
             ps.setString(8, user.getSanction_Reason());
             ps.setString(9, user.getProfile_pic_filepath());
             ps.setString(10, username);
         int rows = ps.executeUpdate();

            ps.close();
            c.close();

            return rows > 0;
            
        } catch (SQLException se) {
            se.printStackTrace();
            return false;
        }   catch(Exception e) {
            e.printStackTrace(System.err);
            return false;
        }

    }

    public Boolean deleteUser(String username) { 
        
        try {
        	Connection c = ConnectionManager.getConnection();
            PreparedStatement ps = c.prepareStatement(userDelete);
                ps.setString(1, username);
            int rows = ps.executeUpdate();

            ps.close();
            c.close();

            return rows > 0;
            
        } catch (SQLException se) {
            se.printStackTrace();
            return false;
        }   catch(Exception e) {
            e.printStackTrace(System.err);
            return false;
        }

    }

}
