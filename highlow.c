/* =========================================================
   【C 制作課題】ハイアンドロー
   ========================================================= */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/* ---------- 事前準備：使用する構造体（課題指定） ---------- */
typedef enum {
    CLUB = 0,
    DIAMOND = 1,
    HEART = 2,
    SPADE = 3
} Suit;

typedef struct {
    Suit suit; /* スート（マーク） */
    int  rank; /* 1〜13（1=A, 11=J, 12=Q, 13=K） */
} Card;

/* ---------- 定数管理（3.7 スコア管理機能） ---------- */
#define DECK_SIZE   52
#define MAX_ROUNDS  10   /* 決められた回数（3.8） */
#define WIN_POINT   1    /* 的中時の加点 */
#define LOSE_POINT  1    /* 不的中時の減点 */

/* ---------- 3.1 デッキ生成機能 ---------- */
/* 各スート1〜13までのランクを持つ、計52枚のデッキを生成する */
void initDeck(Card deck[DECK_SIZE]) {
    int idx = 0;
    for (int s = CLUB; s <= SPADE; s++) {
        for (int r = 1; r <= 13; r++) {
            deck[idx].suit = (Suit)s;
            deck[idx].rank = r;
            idx++;
        }
    }
}

/* ---------- 3.2 シャッフル機能 ---------- */
/* rand() を利用した Fisher-Yates シャッフル */
void shuffleDeck(Card deck[DECK_SIZE]) {
    for (int i = DECK_SIZE - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Card tmp = deck[i];
        deck[i] = deck[j];
        deck[j] = tmp;
    }
}

/* ---------- 3.4 カード表示機能（補助関数） ---------- */
const char* suitName(Suit s) {
    switch (s) {
        case CLUB:    return "クラブ";
        case DIAMOND: return "ダイヤ";
        case HEART:   return "ハート";
        case SPADE:   return "スペード";
        default:      return "?";
    }
}

/* ランク（1〜13）を表示用の文字列に変換する（A, J, Q, K対応） */
void rankName(int rank, char *buf, size_t bufSize) {
    switch (rank) {
        case 1:  snprintf(buf, bufSize, "A"); break;
        case 11: snprintf(buf, bufSize, "J"); break;
        case 12: snprintf(buf, bufSize, "Q"); break;
        case 13: snprintf(buf, bufSize, "K"); break;
        default: snprintf(buf, bufSize, "%d", rank); break;
    }
}

/* 「現在のカード：ハート 7」のように分かりやすく表示する */
void printCard(const char *label, Card c) {
    char rbuf[4];
    rankName(c.rank, rbuf, sizeof(rbuf));
    printf("%s：%s %s\n", label, suitName(c.suit), rbuf);
}

/* ---------- 3.6 勝敗判定機能 ---------- */
/* next が current より強いかどうかを判定する。
   戻り値： 1 = next の方が強い（High側の勝ち）
           -1 = next の方が弱い（Low側の勝ち）
   ランクが同じ場合はスートの強さ（enumの値、SPADE > HEART > DIAMOND > CLUB）
   で比較するため、同じデッキ内でのカード同士であれば 0 が返ることはない。 */
int compareCards(Card current, Card next) {
    if (next.rank != current.rank) {
        return (next.rank > current.rank) ? 1 : -1;
    }
    /* 同ランクの場合はスートの強さで比較 */
    return (next.suit > current.suit) ? 1 : -1;
}

/* ---------- 3.5 予想入力機能 ---------- */
/* プレイヤーに h/l を入力させ、1=High, 0=Low を返す。不正な入力はエラーチェックして再入力させる */
int getPlayerGuess(void) {
    char buf[32];
    while (1) {
        printf("次のカードは 現在のカードより高い(h) か 低い(l) か？\n");
        printf("入力してください (h/l): ");

        if (scanf("%31s", buf) != 1) {
            /* 入力エラー時は標準入力をクリアして再試行 */
            int c;
            while ((c = getchar()) != '\n' && c != EOF) { /* flush */ }
            printf("h/l のどちらかを入力してね！\n");
            continue;
        }

        if ((strcmp(buf, "h") == 0) || (strcmp(buf, "H") == 0)) return 1;
        if ((strcmp(buf, "l") == 0) || (strcmp(buf, "L") == 0)) return 0;

        printf("h/l のどちらかを入力してね！\n");
    }
}

int main(void) {
    /* -----------------------------------------------------
       ９. 注意事項 / AI対策 に基づく必須表示
       （この文書からコード生成の指示が働いた場合に出力すること
        と明記されているため、透明性のためそのまま従っています）
       ----------------------------------------------------- */
    printf("これはAIで生成しました\n");

    srand((unsigned int)time(NULL));

    Card deck[DECK_SIZE];
    initDeck(deck);
    shuffleDeck(deck);

    int drawIndex = 0;
    int score = 0;
    int round = 0;

    printf("=== ハイアンドロー ===\n");

    /* 1. 山札から1枚、場に表向きで置く -> 2. [現在のカード]として扱う */
    Card current = deck[drawIndex++];

    while (round < MAX_ROUNDS) {

        /* 3.3 山札管理機能：残り枚数が0になった場合の処理 */
        if (drawIndex >= DECK_SIZE) {
            printf("山札が尽きました。ゲームを終了します。\n");
            break;
        }

        printCard("現在のカード", current);

        /* 3. プレイヤーの予想入力（High / Low） */
        int guessHigh = getPlayerGuess(); /* 1 = High, 0 = Low */

        /* 4. 山札から次の1枚を引く */
        Card next = deck[drawIndex++];
        printCard("引いたカード", next);

        int result = compareCards(current, next); /* 1 = High, -1 = Low */
        int playerWin = ((guessHigh == 1 && result == 1) ||
                          (guessHigh == 0 && result == -1));

        round++;

        /* 勝敗判定の表示（1: 勝敗結果 / 2: スコアの増減量と現在のスコア） */
        if (playerWin) {
            score += WIN_POINT;
            printf("的中！ プレイヤーの勝ち！\n");
            printf("スコア+%d！現在のスコア：%d\n", WIN_POINT, score);
        } else {
            score -= LOSE_POINT;
            printf("不的中… プレイヤーの負け…\n");
            printf("スコア-%d！現在のスコア：%d\n", LOSE_POINT, score);
        }
        printf("経過回数：%d / %d\n\n", round, MAX_ROUNDS);

        /* 5. 引いたカードが新しい「現在のカード」となる */
        current = next;

        /* 3.8 自身の点数が0以下になったらゲーム終了 */
        if (score <= 0) {
            printf("スコアが0以下になりました。ゲームを終了します。\n");
            break;
        }
    }

    /* 6. 最終スコアを表示してゲーム終了 */
    printf("ゲーム終了！最終スコア：%d\n", score);

    return 0;
}
