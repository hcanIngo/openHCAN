	/* Bsp. input: 'myjp({"92":22})'
        output "ON" "OFF" als String */
(input.substring(input.indexOf(":") + 1, input.lastIndexOf("}")) === "1") ? "ON":"OFF"
