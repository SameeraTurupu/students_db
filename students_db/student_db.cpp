#define _CRT_SECURE_NO_WARNINGS
/*
	Have to include two DB files users and their marks included files.
*/
#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
#include<string.h>

struct Return{
	void * ans;
	int effected_rows;
};

struct stud_data{
	int id;
	char * fName;
	char * lName;
	char * place;
	char * gender;
};

struct Token{
	char * ans;
	bool next;
};

struct stud_marks{
	int id;
	int * marks;
};

struct data_node{
	stud_data data;
	data_node * left;
	data_node * right;
};

struct marks_node{
	stud_marks data;
	marks_node * left;
	marks_node * right;
};

struct piece{
	char * logic;
	char * lhs;
	char * operand;
	char * rhs;
};

struct ans_node{
	data_node * lhs;
	stud_marks rhs;
};
Token * getChars(FILE * fp);
Return * get_subjects_list(FILE *);

char ** addToMultiDimenArray(char ** mda, char * arr, int pos){
	mda = (char **)realloc(mda, sizeof(*mda)* (pos + 1));
	mda[pos] = (char *)malloc(sizeof(char)* 10);
	int i = 0;
	while (arr[i] != '\0')
	{
		mda[pos][i] = arr[i];
		i++;
	}
	mda[pos][i] = '\0';
	return mda;
}

int stringCompare(char * str1, char * str2){
	int i1 = 0, i2 = 0;
	if (str1 == NULL){
		if (str2 == NULL)
			return 0;
		return -1;
	}
	if (str2 == NULL)return -1;
	while (str1[i1] != '\0' || str2[i2] != '\0'){
		if (str1[i1] != str2[i2])
			return str1[i1] - str2[i2];
		i1++; i2++;
	}
	if (str1[i1] == '\0'){
		if (str2[i2] == '\0')
			return 0;
		return -1;
	}
	return 1;
}

int str_to_int(char * str){
	if (str == NULL)
		return 0; int i = 0;
	int sum = 0;
	while (str[i] != 0){
		sum = sum * 10 + str[i] - '0'; i++;
	}
	return sum;
}

Return * get_subjects_list(FILE * fp){
	int len = 0, rows_effected = 0; char ch; bool next;
	char ** subjects = NULL;
	//Getting subject names first
	while (true){
		Token * temp = getChars(fp);
		subjects = addToMultiDimenArray(subjects, temp->ans, len); len++;
		if (!temp->next)break;
	}
	fclose(fp);
	Return * ans = (Return *)malloc(sizeof(Token));
	ans->ans = (void *)subjects;
	ans->effected_rows = len;
	return ans;
}

int get_sub_index(char * subject,Return * subjects){
	char ** subject_list = (char **)subjects->ans;
	for (int i = 0; i < subjects->effected_rows; i++){
		if (stringCompare(subject, subject_list[i]) == 0)
			return i;
	}
	return -1;
}

data_node * get_data_by_id(int id, data_node * root_d){
	if (root_d == NULL || root_d->data.id == id)
		return root_d;

	if (root_d->data.id < id)
		return get_data_by_id(id, root_d->right);

	return get_data_by_id(id, root_d->left);
}

ans_node * retrieve_from_id(stud_marks * rs,int len,data_node * root_d){
	ans_node * ans = (ans_node *)malloc(sizeof(ans_node)*len);
	for (int i = 0; i < len; i++){
		data_node * temp = get_data_by_id(rs[i].id, root_d);
		ans[i].lhs = temp;
		ans[i].rhs = rs[i];
	}
	return ans;
}

char * getCharsFromString(char * str,int * pos){
	char * res = NULL; int * i = pos, len = 0;
	while (true){
		char ch = str[(*pos)++];
		if (ch == '\n' || ch == ' ' || ch == 0 || ch == ',' || ch == -1){
			break;
		}
		else{
			res = (char *)realloc(res, sizeof(char)*(len + 1)); len++;
			res[len - 1] = ch;
		}
	}
	if (len == 0)return NULL;
	res[len] = 0;
	return res;
}

bool if_result_in_set(int id, Return * rs){
	int len = rs->effected_rows;
	stud_marks * result_set = (stud_marks *)rs->ans;
	for (int i = 0; i < len;i++)
	if (result_set[i].id == id)
		return true;
	return false;
}

Return get_result_set(int index, char * operand,char * logic,  int rhs, marks_node * root,Return * prev, stud_marks * ans,int * len)
{
	Return kp = { NULL, 0 };
	if (root == NULL)
		return kp;
	get_result_set(index, operand, logic, rhs, root->left, prev, ans, len);
	//--------------------------------------------------------
	bool satisfied = false;
	if (stringCompare(operand, "=") == 0){
		if (root->data.marks[index] == rhs)
			satisfied = true;
	}
	else if (stringCompare(operand, "<=") == 0){
		if (root->data.marks[index] <= rhs)
			satisfied = true;
	}
	else if (stringCompare(operand, ">=") == 0){
		if (root->data.marks[index] >= rhs)
			satisfied = true;
	}
	else if (stringCompare(operand, "<") == 0){
		if (root->data.marks[index] < rhs)
			satisfied = true;
	}
	else if (stringCompare(operand, ">") == 0){
		if (root->data.marks[index] > rhs)
			satisfied = true;
	}
	else if (stringCompare(operand, "!=") == 0){
		if (root->data.marks[index] != rhs)
			satisfied = true;
	}
	if (satisfied){
		if (stringCompare(logic, "and") == 0){
			if (!if_result_in_set(root->data.id, prev)){
				ans = (stud_marks *)realloc(ans, sizeof(stud_marks)*(*len + 1));
				ans[*len] = root->data;
				(*len)++;
			}
		}
		else{
			ans = (stud_marks *)realloc(ans, sizeof(stud_marks)*(*len + 1));
			ans[*len] = root->data;
			(*len)++;
		}
	}
	//--------------------------------------------------------
	get_result_set(index, operand, logic, rhs, root->right, prev, ans, len);
}

ans_node * process_query(char * query,marks_node * root_m,data_node * root_d)
{
	if (stringCompare(query, "exit") == 0)
		exit(0);
	else if (stringCompare(query, "clear") == 0){
		system("cls");
		return NULL;
	}
	char buffer[100]; int pos = 0; int i = 0, len = 0;
	while (query[pos] != ' ' && query[pos] != '\n' && query[pos] != 0){
		buffer[i++] = query[pos++];
	}
	buffer[i] = 0;
	Return * subjects_adapter = get_subjects_list(fopen("students_marks.txt", "r")); char ** subjects = (char **)subjects_adapter->ans;
	piece * logics = NULL; bool first_time = true;
	if (stringCompare(buffer, "select") == 0){
		while (true){
			if (query[pos] == 0)break;
			while (query[pos] == ' ')pos++;
			char * logic = NULL;
			if (!first_time){
				logic = getCharsFromString(query, &pos);
			}
			else first_time = false;
			char * lhs = getCharsFromString(query, &pos);
			char * operand = getCharsFromString(query, &pos);
			char * rhs = getCharsFromString(query, &pos); pos--;
			logics = (piece *)realloc(logics, sizeof(piece)*(len + 1)); len++;
			logics[len - 1].lhs = lhs;
			logics[len - 1].rhs = rhs;
			logics[len - 1].operand = operand;
			logics[len - 1].logic = logic;
			while (query[pos] == ' ')pos++;
		}Return * prev = NULL; stud_marks * finale = NULL; int finale_len = 0;
		for (int i = 0; i < len; i++){
			stud_marks * ans = NULL; int rows_ = 0;
			if (stringCompare(logics[i].lhs, "name") == 0){

			}
			else{
				int index = get_sub_index(logics[i].lhs, subjects_adapter);
				if (index == -1){
					printf("Invalid subject %s..\n", logics[i].lhs);
					return NULL;
				}stud_marks * ans = (stud_marks *)malloc(sizeof(stud_marks)); int len_ = 0;
				int marks = str_to_int(logics[i].rhs);
				int len_ans = 0;
				get_result_set(index, logics[i].operand, logics[i].logic, marks, root_m, prev, ans, &rows_);
				Return * temp = (Return *)malloc(sizeof(Return));
				temp->ans = ans; temp->effected_rows = rows_;
				ans = (stud_marks *)temp->ans;
				len_ = temp->effected_rows;
				finale_len = len_;
				prev = temp;
				finale = ans;
			}
		}
		ans_node * t = retrieve_from_id(finale, finale_len, root_d);
		if (finale_len == 0){
			printf("0 results found\n");
		}
		else{
			printf("%2s %12s %12s %15s ", "No", "First name", "Last name", "Place");
			char ** subjects = (char **)subjects_adapter->ans;
			for (int i = 0; i < subjects_adapter->effected_rows; i++){
				printf("%9s ", subjects[i]);
			}printf("\n");
			for (int i = 0; i < finale_len; i++){
				printf("%2d %12s %12s %15s ", t[i].lhs->data.id, t[i].lhs->data.fName, t[i].lhs->data.lName, t[i].lhs->data.place);
				for (int j = 0; j < subjects_adapter->effected_rows; j++){
					printf("%9d ", t[i].rhs.marks[j]);
				}printf("\n");
			}
		}
	}
	else
		printf("Syntax error...");
	return NULL;
}

void fback(FILE * fp){ fseek(fp, -1, SEEK_CUR); }

data_node * arr_to_data_tree(stud_data * arr, int start, int end)
{
	if (start > end)
		return NULL;
	int mid = (start + end) / 2;
	data_node * root = (data_node *)malloc(sizeof(data_node));
	root->data = arr[mid];
	root->left = arr_to_data_tree(arr, start, mid - 1);
	root->right = arr_to_data_tree(arr, mid + 1, end);
	return root;
}

marks_node * arr_to_marks_tree(stud_marks * arr, int start, int end)
{
	if (start > end)
		return NULL;
	int mid = (start + end) / 2;
	marks_node * root = (marks_node*)malloc(sizeof(marks_node));
	root->data = arr[mid];
	root->left = arr_to_marks_tree(arr, start, mid - 1);
	root->right = arr_to_marks_tree(arr, mid + 1, end);
	return root;
}

Token * getChars(FILE * fp){
	char * res = NULL; int len = 0;
	Token * ans = (Token *)malloc(sizeof(Token));
	ans->next = true;
	while (true){
		char ch = fgetc(fp);
		if (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z')){
			res = (char *)realloc(res, sizeof(char)*(len + 1)); len++;
			res[len - 1] = ch;
		}
		else if (ch == '\n'){
			ans->next = false;
			break;
		}
		else if (ch == ',' || ch == -1)
			break; else break;
	}
	res[len] = 0;
	ans->ans = res;
	return ans;
}

int getInt(FILE * fp,bool * next){
	int sum = 0; *next = true;
	while (true){
		char ch = fgetc(fp);
		if ('0' <= ch && ch <= '9')
			sum = sum * 10 + (ch - '0');
		else if (ch == -1){
			*next = false; break;
		}
		else if (ch == ',' || ch == '\n')
			break; else break;
	}
	return sum;
}

struct node{
	node * left;
	stud_data data;
	node * right;
};

Return * create_array(FILE * fp){
	stud_data * arr = NULL; int len = 0, count = 0; bool next;
	while (true){
		char ch = fgetc(fp);
		if (ch == -1)break;
		fback(fp);
		len++;
		arr = (stud_data *)realloc(arr, sizeof(stud_data)*len);
		arr[len - 1].id = getInt(fp,&next);
		Token * temp = getChars(fp);
		arr[len - 1].fName = temp->ans;
		temp = getChars(fp);
		arr[len - 1].lName = temp->ans;
		temp = getChars(fp);
		arr[len - 1].place = temp->ans;
		temp = getChars(fp);
		arr[len - 1].gender = temp->ans;
		count++;
	}
	Return * ans = (Return *)malloc(sizeof(Return));
	ans->ans = (void *)arr;
	ans->effected_rows = count;
	return ans;
}

Return * create_marks_array(FILE * fp){
	stud_marks * arr = NULL; int len = 0, rows_effected = 0; char ch; bool next;
	char ** subjects = NULL;
	//Getting subject names first
	while (true){
		Token * temp = getChars(fp);
		subjects = addToMultiDimenArray(subjects, temp->ans, len); len++;
		if (!temp->next)break;
	}
	while (true){
		int id = getInt(fp, &next);
		int * marks = (int *)malloc(sizeof(int)*len);
		for (int i = 0; i < len; i++){
			marks[i] = getInt(fp, &next);
		}
		arr = (stud_marks *)realloc(arr, sizeof(stud_marks)*(rows_effected + 1));
		rows_effected++;
		arr[rows_effected - 1].id = id;
		arr[rows_effected - 1].marks = marks;
		if (next == false)break;
	}
	printf("%d rows effected\n", rows_effected);
	Return * ans = (Return *)malloc(sizeof(Return));
	ans->ans = (void *)arr;
	ans->effected_rows = rows_effected;
	return ans;
}

void query_region(marks_node * root_m, data_node * root_d){
	char query[255];
	printf("Entered query region..\n\n");
	printf("Allowed queries :\n\n");
	printf("select subject_name = marks and/or subject_name = marks ...\n");
	printf("                    |_ =,<=,>=,!=,<,> are allowed          \n\n");
	printf("===========================================================\n\n");
	while (true){
		printf("\n>>");
		gets(query);
		process_query(query, root_m, root_d);
		memset(query, 0, 255);
	}
}

void main(){
	char * users_file_f = "students_data.txt";
	char * stud_marks_f = "students_marks.txt";
	FILE * fp = fopen(users_file_f, "r+");
	FILE * fp1 = fopen(stud_marks_f, "r+");
	Return * temp = create_array(fp);
	stud_data * k = (stud_data *)temp->ans;
	data_node * root_d = arr_to_data_tree(k, 0, temp->effected_rows);
	for (int i = 0; i < temp->effected_rows; i++){
		printf("%2d  %s %s %s %s\n", k[i].id, k[i].fName, k[i].lName,k[i].place,k[i].gender);
	}
	printf("====================================================\n");
	Return * temp1 = create_marks_array(fp1);
	stud_marks * arr = (stud_marks *) temp1->ans;
	marks_node * root_m = arr_to_marks_tree(arr, 0, temp1->effected_rows-1);
	query_region(root_m, root_d);
	_getch();
}