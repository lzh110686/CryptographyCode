

import javax.crypto.Cipher;
import javax.crypto.SecretKey;
import javax.crypto.spec.SecretKeySpec;

public class do3DES {

   //key 根据实际情况对应的修改
   private final byte[] keybyte="123456788765432112345678".getBytes(); //keybyte为加密密钥，长度为24字节
   private static final String Algorithm = "DESede"; //定义 加密算法,可用 DES,DESede,Blowfish
   private SecretKey deskey;
   //生成密钥
   public do3DES(){
       deskey = new SecretKeySpec(keybyte, Algorithm);
   }
   //加密
   public byte[] encrypt(byte[] data){
        try {
            Cipher cipher = Cipher.getInstance("DESede/ECB/PKCS5Padding");
            cipher.init(Cipher.ENCRYPT_MODE, deskey);
            return cipher.doFinal(data);
        } catch (Exception ex) {
            //加密失败，打日志
            ex.printStackTrace();
        } 
        return null;
   }
   //解密
   public byte[] decrypt(byte[] data){
       try {
            Cipher cipher = Cipher.getInstance("DESede/ECB/PKCS5Padding");
            cipher.init(Cipher.DECRYPT_MODE,deskey);
            return cipher.doFinal(data);
        } catch (Exception ex) {
            //解密失败，打日志
            ex.printStackTrace();
        } 
        return null;
   }

   public static void main(String[] args) throws Exception {
	   do3DES des=new do3DES();
       String req ="cryptology";

       String toreq  = toHexString(req);
       System.err.println("十六进制报文=="+toreq);
       byte[] srcData=req.toString().getBytes("utf-8");
       byte[] encryptData=des.encrypt(srcData);
       System.out.println("密文：");
       if(encryptData!=null){
           for(int i=0;i<encryptData.length;i++){
               String hex=Integer.toHexString(encryptData[i]);
               if(hex.length()>1)
                System.out.print(hex.substring(hex.length()-2)+" ");
               else
                System.out.print("0"+hex+" ");
           }
       }
       System.out.println("");
       System.out.println("明文："+req);
   }

   // 转化字符串为十六进制编码
   public static String toHexString(String s) {
       String str = "";
       for (int i = 0; i < s.length(); i++) {
           int ch = (int) s.charAt(i);
           String s4 = Integer.toHexString(ch);
           str = str + s4;
       }
       return str;
   }
}
