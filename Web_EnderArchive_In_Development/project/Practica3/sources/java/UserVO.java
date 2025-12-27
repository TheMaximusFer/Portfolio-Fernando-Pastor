package VO_DAO;
import java.sql.Timestamp;


public final class UserVO {
    private String username;
    private String password;
    private String description;
    private Boolean admin;
    private Boolean sanctioned;
    private Boolean banned;
    private Timestamp end_of_sanction;
    private String sanction_reason;
    private String profile_pic_filepath;
    public UserVO( String username,  String password,  String description,  Boolean admin,  Boolean sanctioned,  Boolean banned, Timestamp end_of_sanction, String sanction_reason, String profile_pic_filepath) {
        this.username = username;
        this.password = password;
        this.description = description;
        this.admin = admin;
        this.sanctioned = sanctioned;
        this.banned = banned;
        this.end_of_sanction = end_of_sanction;
        this.sanction_reason = sanction_reason;
        this.profile_pic_filepath = profile_pic_filepath;
    }
    public String getUsername() { return username; }
    public String getPassword() { return password; }
    public String getDescription() { return description; }
    public Boolean getAdmin() { return admin; }
    public Boolean getSanctioned() { return sanctioned; }
    public Boolean getBanned() { return banned; }
    public Timestamp getEndOfSanction() { return end_of_sanction; }
    public String getSanction_Reason() { return sanction_reason; }
    public String getProfile_pic_filepath() {return profile_pic_filepath;}

    public void setUsername(String username) { this.username = username; }
    public void setPassword(String password) { this.password = password; }
    public void setDescrption(String description) { this.description = description; }
    public void setAdmin(Boolean admin) { this.admin = admin; }
    public void setSanctioned(Boolean sanctioned) { this.sanctioned = sanctioned; }
    public void setBanned(Boolean banned) { this.banned = banned; }
    public void setEndOfSanction(Timestamp end_of_sanction) { this.end_of_sanction = end_of_sanction; }
    public void setSanction_Reason(String sanction_reason) {  this.sanction_reason = sanction_reason; }
    public void setProfile_pic_filepath(String profile_pic_filepath) {this.profile_pic_filepath = profile_pic_filepath;}
}