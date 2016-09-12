package com.smackall.iyan3dPro.Helper;


public class AssetsDB {

    private String assetName;
    private int assetsId;
    private String assetPath;
    private String hash;
    private int iap;
    private Integer id;
    private String keywords;
    private int nBones;
    private String time;
    private int type;
    private int group;
    private boolean isTempNode = true;
    private String texture = "";
    private float width = 0;
    private float height = 0;
    private float x = -1.0f;
    private float y = -1.0f;
    private float z = -1.0f;
    private int actionType = Constants.IMPORT_ASSET_ACTION;
    private boolean hasMeshColor = false;
    private String texturePath = PathManager.LocalTextureFolder + "/";


    public AssetsDB() {
    }

    public AssetsDB(int n, int n2) {
        this.assetsId = n;
        this.type = n2;
    }

    public AssetsDB(Integer n, String string2, int n2, int n3, int n4, int n5, String string3, String string4, String string5, int group) {
        this.id = n;
        this.assetName = string2;
        this.iap = n2;
        this.assetsId = n3;
        this.type = n4;
        this.nBones = n5;
        this.keywords = string3;
        this.hash = string4;
        this.time = string5;
        this.group = group;
    }

    public AssetsDB(String string2, int n2, int n3, int n4, int n5, String string3, String string4, String string5, int group) {
        this.assetName = string2;
        this.iap = n2;
        this.assetsId = n3;
        this.type = n4;
        this.nBones = n5;
        this.keywords = string3;
        this.hash = string4;
        this.time = string5;
        this.group = group;
    }

    public AssetsDB(String string2, int n, int n2, int n3, int n4, String string3, String string4, String string5) {
        this.assetName = string2;
        this.iap = n;
        this.assetsId = n2;
        this.type = n3;
        this.nBones = n4;
        this.keywords = string3;
        this.hash = string4;
        this.time = string5;
    }

    public void resetValues() {
        assetName = "";
        assetsId = 0;
        assetPath = "";
        hash = "";
        iap = 0;
        id = 0;
        keywords = "";
        nBones = 0;
        time = "";
        type = 0;
        group = 0;
        isTempNode = true;
        texture = "";
        width = 0;
        height = 0;
        x = -1.0f;
        y = -1.0f;
        z = -1.0f;
        actionType = Constants.IMPORT_ASSET_ACTION;
        hasMeshColor = false;
        texturePath = PathManager.LocalTextureFolder + "/";
    }

    public int getGroup() {
        return this.group;
    }

    public void setGroup(int n) {
        this.group = n;
    }

    public String getTexture() {
        return this.texture;
    }

    public void setTexture(String texture) {
        this.texture = texture;
    }

    public boolean getIsTempNode() {
        return this.isTempNode;
    }

    public void setIsTempNode(boolean isTempNode) {
        this.isTempNode = isTempNode;
    }

    public String getAssetName() {
        return this.assetName;
    }

    public void setAssetName(String string2) {
        this.assetName = string2;
    }

    public int getAssetsId() {
        return this.assetsId;
    }

    public void setAssetsId(int n) {
        this.assetsId = n;
    }

    public String getDateTime() {
        return this.time;
    }

    public void setDateTime(String string2) {
        this.time = string2;
    }

    public String getHash() {
        return this.hash;
    }

    public void setHash(String string2) {
        this.hash = string2;
    }

    public int getID() {
        return this.id;
    }

    public void setID(int n) {
        this.id = n;
    }

    public int getIap() {
        return this.iap;
    }

    public void setIap(int n) {
        this.iap = n;
    }

    public String getKeywords() {
        return this.keywords;
    }

    public void setKeywords(String string2) {
        this.keywords = string2;
    }

    public float getWidth() {
        return this.width;
    }

    public void setWidth(float width) {
        this.width = width;
    }

    public float getHeight() {
        return this.height;
    }

    public void setHeight(float height) {
        this.height = height;
    }

    public int getNBones() {
        return this.nBones;
    }

    public void setNBones(int n) {
        this.nBones = n;
    }

    public int getType() {
        return this.type;
    }

    public void setType(int n) {
        this.type = n;
    }

    public float getX() {
        return this.x;
    }

    public void setX(float x) {
        this.x = x;
    }

    public float getY() {
        return this.y;
    }

    public void setY(float y) {
        this.y = y;
    }

    public float getZ() {
        return this.z;
    }

    public void setZ(float z) {
        this.z = z;
    }

    public int getActionType() {
        return this.actionType;
    }

    public void setActionType(int actionType) {
        this.actionType = actionType;
    }

    public String getAssetPath() {
        return this.assetPath;
    }

    public void setAssetPath(String path) {
        this.assetPath = path;
    }

    public boolean isHasMeshColor() {
        return hasMeshColor;
    }

    public void setHasMeshColor(boolean hasMeshColor) {
        this.hasMeshColor = hasMeshColor;
    }

    public String getTexturePath() {
        return texturePath;
    }

    public void setTexturePath(String texturePath) {
        this.texturePath = texturePath;
    }
}


