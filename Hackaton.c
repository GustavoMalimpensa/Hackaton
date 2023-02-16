#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

typedef struct PRODUTO{
	int cod;
	char nome[20];
    float preco;
   	int quantidade_atual;
    int quantidade_vendida;
}PRODUTO;

typedef struct ANUNCIO{
	bool status;
	int code;
	char nome[20];
	float preco;
}ANUNCIO;

typedef struct REG_COMPRAS{
	char nome[20];
	int quantidade;
	float valor;
}REG_COMPRAS;

typedef struct MARKETPLACE{
    char nome[50];
	ANUNCIO anuncios[10];
	REG_COMPRAS registro[20];
}MARKETPLACE;

//----------protipagem de funcoes----------
FILE* abre_arquivo(char* diretorio);
void cadastra_produto(PRODUTO *vetProduto, int tam);
void remove_anuncio(MARKETPLACE *sistema, int tam);
void insere_MarketPlace(MARKETPLACE *vetMkp, PRODUTO *estoque, int tamEstoque, int tamSistema);
void carrega_MarketPlace(FILE* fp, MARKETPLACE *vetMkp, int pivo);
void carrega_estoque(FILE* fp, PRODUTO *estoque, int tamEstoque);
void comprar_produto(MARKETPLACE *vetMkp, PRODUTO *estoque, int tamEstoque);
void busca_produto(PRODUTO *estoque, int tamEstoque);
void busca_compra(MARKETPLACE *vetMkp, int tamSistema);

int main(void) {
	
	const int tamEstoque = 15, tamSistema = 2;
	int i, j, escolha;
	PRODUTO estoque[tamEstoque];
	MARKETPLACE sistema[tamSistema];
	//iniciar anuncios vagos
	for(i=0;i<tamEstoque;i++){
		estoque[i].cod=0;
	}
	for(i=0;i<tamSistema;i++){
		for(j=0;j<10;j++){
			sistema[i].anuncios[j].status=false;
			sistema[i].anuncios[j].code=0;
		}
		for(j=0;j<20;j++){
			sistema[i].registro[j].valor=0;
		}
	}
	
	FILE *erp, *mk1, *mk2; 
	//Iniciar arquivos do ERP e Marketplace
	erp = abre_arquivo("erp.bin");
	carrega_estoque(erp, estoque, tamEstoque);
	mk1 = abre_arquivo("marketplace1.bin");
	carrega_MarketPlace(mk1, sistema, 1);
	mk2 = abre_arquivo("marketplace2.bin");
	carrega_MarketPlace(mk2, sistema, 2);
	rewind(erp);
	rewind(mk1);
	rewind(mk2);
	do{
		printf("\nEscolha:\n1)Cadastra Produto\n2)Inserir Anuncio\n3)Remover Anuncio\n4)Comprar Produto\n5)Busca Produto\n6)Busca Compra\n0)Sair da Aplicacao\n");
		scanf("%d", &escolha);
		switch(escolha){
			case 1:
				printf("---------------------------\n");
				cadastra_produto(estoque, tamEstoque);
				printf("---------------------------\n");
				break;
			case 2:
				printf("---------------------------\n");
				insere_MarketPlace(sistema, estoque, tamEstoque, tamSistema);
				printf("---------------------------\n");
				break;
			case 3:
				printf("---------------------------\n");
				remove_anuncio(sistema, tamSistema);
				printf("---------------------------\n");
				break;
			case 4:
				printf("---------------------------\n");
				comprar_produto(sistema, estoque, tamEstoque);
				printf("---------------------------\n");
				break;
			case 5: 
				printf("---------------------------\n");
				busca_produto(estoque, tamEstoque);
				printf("---------------------------\n");
				break;
			case 6:
				printf("---------------------------\n");
				busca_compra(sistema, tamSistema);
				printf("---------------------------\n");
				break;
			case 0:
				//Escrevendo o estoque atual
				fwrite(&estoque, sizeof(PRODUTO), tamEstoque, erp);
				//Escrevendo marketplace 1
				fwrite(&sistema[0], sizeof(MARKETPLACE), 1, mk1);
				//Escrevendo marketplace 2
				fwrite(&sistema[1], sizeof(MARKETPLACE), 1, mk2);
				printf("Encerrando sistema de gerenciamento....");
				fclose(erp);
				fclose(mk1);
				fclose(mk2);
				return 0;
				break;
			default:
				printf("Escolha invalida!!");
				break;
		}
	}while(true);
	
	return;
}

//---------Funcoes------------

FILE *abre_arquivo(char *diretorio){
    FILE *pont;

    if (access(diretorio, F_OK) == 0)
    {
        pont = fopen(diretorio, "r+b");
    }
    else
    {
        pont = fopen(diretorio, "w+b");
    }

    return pont;
}

void insere_MarketPlace(MARKETPLACE *vetMkp, PRODUTO *estoque, int tamEstoque, int tamSistema){
	ANUNCIO anuncio;
	int i, j, escolha;
	char name[20];
	PRODUTO pivo;
	bool existe=false, livre=false, stat=false;

	printf("Qual o nome do produto que ira anunciar?\n");
	scanf("%s", name);

	//Buscar informacoes do produto no estoque do ERP
	for(i=0;i<tamEstoque;i++){
		if(strcmp(estoque[i].nome, name)==0){
			existe=true;
			pivo=estoque[i];
		}
	}
	if(existe==false){
		printf("Produto nao existe no estoque!!\n");
		return;
	}

	//Passar as informacoes relevantes do produto para o anuncio 
	anuncio.code=pivo.cod;
	strcpy(anuncio.nome, pivo.nome);
	anuncio.preco=pivo.preco;
	anuncio.status=true;
	//Decide em qual marketplace sera anunciado
	printf("Em qual marketplace?(O numero dele(1,2,3...), ou para inserir em todos escolha 0)");
	scanf("%d", &escolha);
	if(escolha==0){
		for(i=0;i<tamSistema;i++){
			livre=false;
			stat=false;
			//checar se o anuncio do produto em questao ja existe no marketplcace
			for(j=0;j<10;j++){
				if(vetMkp[i].anuncios[j].code==anuncio.code){
					printf("Produto ja anunciado no marketplace %d\n", i+1);
					stat=true;
					break;
				}
			}
			
			//Buscar espaco livre para o anuncio 
			if(stat==false){
				for(j=0;j<10;j++){
					if(vetMkp[i].anuncios[j].status==false){
						printf("Anunciado no marketplace %d com sucesso!!\n", i+1);
						livre=true;
						vetMkp[i].anuncios[j]=anuncio;
						break;
					}
				}
				if(livre==false){
					printf("Nao tem espaco disponivel para anuncio no marketplace %d\n", i+1);
				}
			}
		}
	}
	else{
		//checar se o anuncio do produto em questao ja existe no marketplace
		for(j=0;j<10;j++){
			if(vetMkp[escolha-1].anuncios[j].code==anuncio.code){
				printf("Produto ja anunciado no marketplace %d\n", escolha);
				stat=true;
				break;
			}
		}
		//Buscar espaco livre para o anuncio
		if(stat==false){
			for(j=0;j<10;j++){
				if(vetMkp[escolha-1].anuncios[j].status==false){
					printf("Anunciado no marketplace %d com sucesso!!\n", escolha);
					livre=true;
					vetMkp[escolha-1].anuncios[j]=anuncio;
					break;
				}
			}
			if(livre==false){
				printf("Nao tem espaco disponivel para anuncio no marketplace %d\n", escolha);
			}
		}
	}
	return;	
}

void carrega_MarketPlace(FILE* fp, MARKETPLACE *vetMkp, int pivo){
	if(feof(fp)) {
		return;
	}
	else {
		fread(&vetMkp[pivo-1], sizeof(MARKETPLACE), 1, fp);
	}
	return;
}

void remove_anuncio(MARKETPLACE *sistema, int tam){
	int i, j;
	int aux;
	
	printf("insira o codigo do anuncio a ser removido:(sera removido de todos os marketplace)");
	scanf("%d", &aux);
	
	for(i=0; i<2; i++) {
		for(j=0; j<10; j++) {
			if(sistema[i].anuncios[j].code == aux) {
				printf("Anuncio removido com sucesso do marketplace %d\n", i+1);
				sistema[i].anuncios[j].status = false;
				break;
			}
		}
	}

	return;
}

void carrega_estoque(FILE* fp, PRODUTO *estoque, int tamEstoque){
	int i;
	PRODUTO pivo;
	fread(estoque, sizeof(PRODUTO), tamEstoque, fp);
	/*while(fread(&pivo, sizeof(PRODUTO), 1, fp)){
		estoque[i]=pivo;
		i++;
	};*/
	return;
}

void cadastra_produto(PRODUTO *vetProduto, int tam) {
	int i;
	
	for(i = 0; i<tam; i++) {
		if(vetProduto[i].cod==0){
			printf("entre com o nome do produto:");
			scanf("%s", vetProduto[i].nome);
			printf("entre com o codigo do protudo:");
			scanf("%d", &vetProduto[i].cod);
			printf("preco:");
			scanf("%f", &vetProduto[i].preco);
			printf("quantidade:");
			scanf("%d", &vetProduto[i].quantidade_atual);
			vetProduto[i].quantidade_vendida=0;
			break;
		}
	}
}

void comprar_produto(MARKETPLACE *vetMkp, PRODUTO *estoque, int tamEstoque){
	PRODUTO pivoProduto;
	char name[20];
	int escolha, escolhaProd, i, j, quantidade, decisao;

	printf("Qual marketplace?(1 ou 2)");
	scanf("%d", &escolha);

	for(i=0;i<10;i++){
		if(vetMkp[escolha-1].anuncios[i].status==true){
			printf("Nome:%s\n", vetMkp[escolha-1].anuncios[i].nome);
			printf("Codigo:%d\n", vetMkp[escolha-1].anuncios[i].code);
			printf("Preco:%f\n\n", vetMkp[escolha-1].anuncios[i].preco);
		}
	}

	printf("Qual o codigo do produto desejado:");
	scanf("%d", &escolhaProd);
	printf("Quantidade desejada:");
	scanf("%d", &quantidade);
	for(i=0;i<tamEstoque;i++){
		if(estoque[i].cod==escolhaProd){
			if(estoque[i].quantidade_atual<quantidade){
				printf("Quantidade insufiente, temos %d, aperte 0 para cancelar compra e 1 pra comprar todos", estoque[i].quantidade_atual);
				scanf("%d", &decisao);
				if(decisao==0){
					printf("Finalizando compra..");
					return;
				}
				else{
					printf("Compra realizada!!");
					for(j=0;j<20;j++){
						if(vetMkp[escolha-1].registro[j].valor==0){
							strcpy(vetMkp[escolha-1].registro->nome,estoque[i].nome);
							vetMkp[escolha-1].registro->quantidade=estoque[i].quantidade_atual;
							vetMkp[escolha-1].registro->valor=estoque[i].quantidade_atual*estoque[i].preco;
							break;
						}
					}
					estoque[i].quantidade_vendida=estoque[i].quantidade_vendida+estoque[i].quantidade_atual;
					estoque[i].quantidade_atual=0;
				}
			}
			else{
				printf("Compra realizada!!");
				estoque[i].quantidade_atual=estoque[i].quantidade_atual-quantidade;
				estoque[i].quantidade_vendida=estoque[i].quantidade_vendida+quantidade;
				for(j=0;j<20;j++){
					if(vetMkp[escolha-1].registro[j].valor==0){
						strcpy(vetMkp[escolha-1].registro->nome,estoque[i].nome);
						vetMkp[escolha-1].registro->quantidade=quantidade;
						vetMkp[escolha-1].registro->valor=quantidade*estoque[i].preco;
						break;
					}
				}
			}
		}
	}
}

void busca_produto(PRODUTO *estoque, int tamEstoque){
	char name[20];
	int i;
	printf("Nome do produto buscado:");
	scanf("%s", name);

	for(i=0;i<tamEstoque;i++){
		if(strcmp(estoque[i].nome,name)==0){
			printf("Nome:%s\n", estoque[i].nome);
			printf("Codigo:%d\n", estoque[i].cod);
			printf("Preco:%f\n", estoque[i].preco);
			printf("Quantidade Atual:%d\n", estoque[i].quantidade_atual);
			printf("Quantidade Vendida:%d\n\n", estoque[i].quantidade_vendida);
		}
	}
}

void busca_compra(MARKETPLACE *vetMkp, int tamSistema){
	char name[20];
	int i, j;
	printf("Nome do produto buscado:");
	scanf("%s", name);

	for(i=0;i<tamSistema;i++){
		printf("-----Marketplace%d-----\n", i+1);
		for(j=0;j<20;j++){
			if(strcmp(vetMkp[i].registro[j].nome,name)==0){
				printf("Nome:%s\n", vetMkp[i].registro[j].nome);
				printf("Quantidade:%d\n", vetMkp[i].registro[j].quantidade);
				printf("Preco Total:%f\n\n", vetMkp[i].registro[j].valor);
			}
		}
	}
}
