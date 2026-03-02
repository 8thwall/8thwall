class HelloJava {
    public static void main(String[] args) {
        System.out.println("Hello, World!"); 
        System.out.print("java.version: ");
        System.out.println(System.getProperty("java.version"));
        System.out.print("java.specification.version: ");
        System.out.println(System.getProperty("java.specification.version"));
    }
}
