# Bem vindo ao Lane Rush

Este é um jogo de carro muito divertido e leve que pode rodar facilmente em seu PC, basta seguir as instruções.

# Instruções

1- Acessar o VS Code

2- instalar as bibliotecas caso necessário

3- Rodar os seguintes comandos:

 conan profile detect --force
 
 export CMAKE_POLICY_VERSION_MINIMUM=3.5 
 
 conan install . --output-folder=build --build=missing
 
 cd build
 
 cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
 
 cmake --build .

# Caso tenha problemas para rodar

Consulte uma Inteligência Artificial (Gemini, ChatGPT, Grok, Claude)
