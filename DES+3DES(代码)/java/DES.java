package decrypt01;
import java.util.Base64;
import java.security.SecureRandom;
import java.security.spec.KeySpec;
import java.util.Base64;

import javax.crypto.Cipher;
import javax.crypto.SecretKey;
import javax.crypto.SecretKeyFactory;
import javax.crypto.spec.DESKeySpec;

public final class DES {

    private static final String ALGORITHM = "DES";
    private static final String TRANSFORMATION = "DES/ECB/PKCS5Padding";

    //利用8个字节64位的key给src加密
    public static byte[] encrypt(byte[] src,byte[]key)
    {
        try {
            Cipher cipher = Cipher.getInstance(TRANSFORMATION);
            SecretKeyFactory secretKeyFactory = SecretKeyFactory.getInstance(ALGORITHM);
            KeySpec keySpec = new DESKeySpec(key);
            SecretKey secretKey = secretKeyFactory.generateSecret(keySpec);
            cipher.init(Cipher.ENCRYPT_MODE, secretKey,new SecureRandom());
            byte[] enMsgBytes = cipher.doFinal(src);    
            return enMsgBytes;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }

    //利用8个字节64位的key给src解密
    public static byte[] decrypt(byte[] encryptBytes,byte[]key){
        try {
            Cipher deCipher = Cipher.getInstance(TRANSFORMATION);
            SecretKeyFactory deDecretKeyFactory = SecretKeyFactory.getInstance(ALGORITHM);
            KeySpec deKeySpec = new DESKeySpec(key);
            SecretKey deSecretKey = deDecretKeyFactory.generateSecret(deKeySpec);
            deCipher.init(Cipher.DECRYPT_MODE, deSecretKey,new SecureRandom());
            byte[] deMsgBytes = deCipher.doFinal(encryptBytes);
            return deMsgBytes;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }
	public static String encode(byte[] key) {
	    return new String(Base64.getEncoder().encode(key));
	    //����base64���ܰ�
	}
	public static String decode(byte[] key) {
	    return new String(Base64.getDecoder().decode(key));
	}
    private static String key = "12345678";

    public static void main(String[] args) throws Exception{
        String msg = "12345678";
        System.out.println("加密前："+msg);
       byte[] encryptBytes = DES.encrypt(msg.getBytes(),key.getBytes());
        System.out.println("加密后："+new String(encryptBytes));
        byte[] deMsgBytes = DES.decrypt(encryptBytes,key.getBytes());
        System.out.println("解密后："+new String(deMsgBytes));
    }

}
