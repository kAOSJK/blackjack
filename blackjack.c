#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

typedef struct card card;
struct card
{
	int value;
	char *symbol;
};

typedef struct scores scores;
struct scores
{
	bool is_game_finished;
	bool is_player_winner;
};

void welcome_message();
void choose_bet_amount(int money);
void ft_putnbr(int nb);
char *randomize_symbol();
bool has_been_drawn(struct card new_card);
int randomize_value();
struct card randomize_card();
void print_card(struct card card);
char *symbol_to_uni(char *symbol);
bool has_been_drawn(struct card new_card);
void initialize_cards();
void add_card(int index, struct card new_card);
void play_game(int money, int bet);
char *value_to_string(int value);
void display_score(int player_score, int croupier_score);
int play_hit(int score, bool is_player);
int check_score_card_value(int score, int value);
void ask_choice(int player_score, int croupier_score, bool standed, int money, int bet);
struct scores check_win(int player_score, int croupier_score, bool standed);
int croupier_turn(int croupier_score);
bool ask_retry(int money);

struct card cards[52];

#define heart "\xE2\x99\xA5"
#define spade "\xE2\x99\xA0"
#define diamond "\xE2\x99\xA6"
#define club "\xE2\x99\xA3"

#define RED " \033[31m"
#define RESET " \033[0m"

int main()
{
	int money = 100;
	welcome_message();
	choose_bet_amount(money);
}

void welcome_message()
{
	puts(" ___________________________");
	puts("| Welcome to BlackJack Game |");
	puts(" ---------------------------\n");
	sleep(1);
}

void choose_bet_amount(int money)
{
	int bet;

	puts("How much money do you want to bet\n");
	scanf("%d", &bet);
	if (bet > money)
	{
		puts("\nCan't afford this amount !");
		puts("Please retry\n");
		sleep(1);
		choose_bet_amount(money);
	}
	else if (bet < 0)
	{
		puts("\nCan't bet negative number");
		puts("Please retry\n");
		sleep(1);
		choose_bet_amount(money);
	}
	else
	{
		puts(" ___________________________");
		printf("| you beted an amount of ");
		printf("%d", bet);
		puts(" |");
		puts(" ---------------------------\n");
		money -= bet;
	}

	play_game(money, bet);
}

void play_game(int money, int bet)
{
	int player_score;
	int croupier_score;

	player_score = 0;
	croupier_score = 0;
	initialize_cards();

	// Drawn first card
	card first_card = randomize_card();
	fputs("Croupier get a ", stdout);
	print_card(first_card);
	croupier_score = check_score_card_value(croupier_score, first_card.value);

	ask_choice(player_score, croupier_score, false, money, bet);
}

void ask_choice(int player_score, int croupier_score, bool standed, int money, int bet)
{
	char choice[8];
	scores win;

	sleep(1);

	if (!standed)
	{
		puts("\nType \"hit\" to hit a new card, \"stand\" to stop here or \"score\" to get the score");

		scanf("\n%s", choice);

		if (choice[0] == 'h')
			player_score = play_hit(player_score, true);
		else if (choice[0] == 's' && choice[1] != 'c')
			standed = true;
		else if (choice[1] == 'c')
			display_score(player_score, croupier_score);
	}

	win = check_win(player_score, croupier_score, standed);

	if (!win.is_game_finished && !standed)
		ask_choice(player_score, croupier_score, standed, money, bet);
	else if (!win.is_game_finished && standed)
	{
		croupier_score = croupier_turn(croupier_score);
		win = check_win(player_score, croupier_score, standed);

		if (!win.is_game_finished)
			ask_choice(player_score, croupier_score, standed, money, bet);
	}

	if (win.is_game_finished)
	{
		if (win.is_player_winner)
			money += (bet * 2);
		if (ask_retry(money))
		{
			initialize_cards();
			choose_bet_amount(money);
		}
	}
}

bool ask_retry(int money)
{
	char choice; // Represent the choice of the player
	char str[4]; // String of the money account

	sprintf(str, "%d", money);

	fputs("------------", stdout);
	fputs("\n\nYour wallet contains ", stdout);
	fputs(str, stdout);
	puts(" coins");

	if (money > 0)
	{
		puts("\nContinue playing ? Y/n");
		scanf(" %c", &choice);
	}
	else
		puts("\nNo money left");

	if (choice == 'Y')
		return true;
	else
		return false;
}

int croupier_turn(int croupier_score)
{
	sleep(1);

	if (croupier_score < 17)
		croupier_score = play_hit(croupier_score, false);

	return croupier_score;
}

struct scores check_win(int player_score, int croupier_score, bool standed)
{
	scores this;

	if (player_score == 21)
	{
		puts("PLAYER WON");
		this.is_game_finished = true;
		this.is_player_winner = true;
	}
	else if (player_score > 21)
	{
		puts("PLAYER LOST");
		this.is_game_finished = true;
		this.is_player_winner = false;
	}
	else if (croupier_score == 21)
	{
		puts("PLAYER LOST");
		this.is_game_finished = true;
		this.is_player_winner = false;
	}
	else if (croupier_score > 21)
	{
		puts("PLAYER WON");
		this.is_game_finished = true;
		this.is_player_winner = true;
	}
	else if (croupier_score > 17 && player_score >= croupier_score && standed)
	{
		puts("PLAYER WON");
		this.is_game_finished = true;
		this.is_player_winner = true;
	}
	else if (standed && player_score < croupier_score)
	{
		puts("PLAYER LOST");
		this.is_game_finished = true;
		this.is_player_winner = false;
	}
	else
	{
		this.is_game_finished = false;
		this.is_player_winner = false;
	}

	return this;
}

int play_hit(int score, bool is_player)
{
	int score_to_add;
	card drawned_card = randomize_card();

	while (has_been_drawn(drawned_card))
	{
		drawned_card = randomize_card();
	}

	if (is_player)
		fputs("\nPlayer get a ", stdout);
	else
		fputs("\nCroupier get a ", stdout);

	print_card(drawned_card);

	fputs("\n", stdout);

	score_to_add = check_score_card_value(score, drawned_card.value);

	return score += score_to_add;
}

void initialize_cards()
{
	int i;

	for (i = 0; i < 52; i++)
	{
		cards[i].value = -1;
		cards[i].symbol = "empty";
	}
}

void print_card(struct card card)
{
	if (card.value <= 10)
		ft_putnbr(card.value);
	else
		fputs(value_to_string(card.value), stdout);

	// Change text color
	if (strcmp(card.symbol, "Hearts") == 0 ||
		strcmp(card.symbol, "Diamonds") == 0)
		fputs(RED, stdout);
	else
		fputs(RESET, stdout);

	puts(symbol_to_uni(card.symbol)); // Change to unicode symbol
	fputs(RESET, stdout);			  // Reset text color
}

struct card randomize_card()
{
	card this;

	this.value = randomize_value();
	this.symbol = randomize_symbol();

	return this;
}

char *randomize_symbol()
{
	int num;

	srand(time(0));
	num = rand() % 4;
	if (num == 0)
		return "Hearts";
	else if (num == 1)
		return "Spades";
	else if (num == 2)
		return "Clubs";
	else
		return "Diamonds";
}

int randomize_value()
{
	int num;

	srand(time(0));
	num = (rand() % 13) + 1;
	return num;
}

bool has_been_drawn(struct card new_card)
{
	int i;
	bool found_card = false;
	int index;

	for (i = 0; i < 52; i++)
	{
		if (cards[i].value == new_card.value &&
			cards[i].symbol == new_card.symbol)
			found_card = true;
		else if (strcmp(cards[i].symbol, "empty") == 0 &&
				 cards[i].value == -1)
			index = i;
	}

	if (found_card)
		return true;
	else // Card has not been drawn
	{
		add_card(index, new_card); // Add card to cards
		return false;
	}
}

void add_card(int index, struct card new_card)
{
	cards[index].symbol = new_card.symbol;
	cards[index].value = new_card.value;
}

void display_score(int player_score, int croupier_score)
{
	fputs("\nYour score is: ", stdout);
	ft_putnbr(player_score);

	fputs("\n", stdout);

	fputs("Croupier score is: ", stdout);
	ft_putnbr(croupier_score);
	fputs("\n", stdout);
}

int check_score_card_value(int score, int value)
{
	int final_value;

	if (value == 1)
	{
		if ((score + 11) > 21)
			final_value = 1;
		else
			final_value = 11;
	}
	else if (value > 10)
		final_value = 10;
	else
		final_value = value;

	return final_value;
}

char *symbol_to_uni(char *symbol)
{
	if (strcmp(symbol, "Hearts") == 0)
		return heart;
	else if (strcmp(symbol, "Spades") == 0)
		return spade;
	else if (strcmp(symbol, "Clubs") == 0)
		return club;
	else if (strcmp(symbol, "Diamonds") == 0)
		return diamond;

	return NULL;
}

char *value_to_string(int value)
{
	if (value == 11)
		return "Jack";
	else if (value == 12)
		return "Queen";
	else
		return "King";
}

void ft_putnbr(int nb)
{
	if (nb < 0)
		nb = -nb;
	if (nb >= 10)
	{
		ft_putnbr(nb / 10);
		ft_putnbr(nb % 10);
	}
	else
		putchar(nb + 48);
}
