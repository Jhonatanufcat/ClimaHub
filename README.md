# ClimaHub
### Descrição do Projeto

Este projeto visa o desenvolvimento de um sistema de controle climático avançado para um climatizador, utilizando uma Placa de Circuito Impresso (PCB) personalizada que integra diversos componentes de hardware e software. O objetivo principal é criar um sistema eficiente, confiável e fácil de usar, capaz de monitorar e ajustar as condições climáticas em tempo real. A seguir, estão os detalhes e componentes-chave do projeto:

#### Componentes do Sistema

1. **Microcontroladores**: Dois microcontroladores são usados para gerenciar as operações do climatizador. Eles são responsáveis pelo processamento de dados, controle de atuadores e comunicação entre os diferentes módulos do sistema.
2. **Sensor de Temperatura BME280**: Este sensor monitora a temperatura e a umidade do ambiente, fornecendo dados precisos e em tempo real para os microcontroladores.
3. **Motor de Passo**: Utilizado para ajustar os componentes mecânicos do climatizador, permitindo um controle preciso do fluxo de ar e outras funções mecânicas.
4. **Relés de Acionamento**: Controlam dispositivos elétricos dentro do climatizador, como ventiladores e compressores.
5. **Visor LCD**: Exibe informações críticas sobre o funcionamento do climatizador e permite a interação direta com o usuário.
6. **Rede Wi-Fi e Protocolo Socket**: Implementados para comunicação entre os dois microcontroladores, superando as limitações da comunicação serial tradicional (TX/RX).
7. **Página HTML para Gerenciamento**: Desenvolvida para permitir o controle remoto do climatizador via uma interface web amigável.
8. **Conexão à Internet e Sinric Pro**: Permitem que o dispositivo envie dados para servidores externos para análise adicional e integração com a assistente virtual Alexa para controle por voz.

#### Funcionalidades do Sistema

- **Monitoramento em Tempo Real**: O sensor BME280 fornece dados contínuos sobre a temperatura e a umidade do ambiente, que são processados pelos microcontroladores para ajustar o funcionamento do climatizador conforme necessário.
- **Controle Preciso de Atuadores**: O motor de passo e os relés são controlados de maneira precisa para ajustar o fluxo de ar e acionar dispositivos elétricos conforme as necessidades detectadas pelos sensores.
- **Interface de Usuário Intuitiva**: O visor LCD e a página HTML permitem que os usuários monitorem e controlem o climatizador de maneira intuitiva, seja localmente ou remotamente.
- **Comunicação Confiável**: A rede Wi-Fi usando protocolo socket assegura uma comunicação robusta e estável entre os microcontroladores, melhorando a confiabilidade do sistema.
- **Controle por Voz**: A integração com Sinric Pro e Alexa permite que os usuários controlem o climatizador por comandos de voz, oferecendo uma experiência de uso moderna e conveniente.
- **Conectividade e Análise de Dados**: A conexão à internet permite que os dados do climatizador sejam enviados para servidores externos, possibilitando análises avançadas e manutenção preditiva.

#### Benefícios do Projeto

- **Eficiência Energética**: O sistema ajusta automaticamente suas operações para otimizar o consumo de energia, resultando em uma operação mais sustentável.
- **Melhoria na Qualidade do Ar**: Com o controle preciso das condições climáticas, o sistema melhora a qualidade do ar interno, proporcionando um ambiente mais saudável e confortável.
- **Acessibilidade e Conveniência**: A interface web e o controle por voz tornam o sistema acessível e fácil de usar, mesmo para pessoas com mobilidade reduzida.
- **Escalabilidade**: A estrutura do sistema permite futuras expansões e integrações com outros dispositivos IoT, garantindo que o projeto possa evoluir com as novas tecnologias.

Este projeto exemplifica como a integração de componentes avançados de hardware e software pode resultar em um sistema de controle climático inteligente, eficiente e amigável. Através da combinação de precisão técnica, comunicação confiável e interfaces de usuário modernas, este projeto estabelece um novo padrão para sistemas de climatização residencial e comercial.
