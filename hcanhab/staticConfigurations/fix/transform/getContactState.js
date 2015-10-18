/* Bsp. input: 'myjp({"92":22})'
        output "OPEN" "CLOSED" */
(input.substring(input.indexOf(":") + 1, input.lastIndexOf("}")) === "1") ? "OPEN":"CLOSED"
