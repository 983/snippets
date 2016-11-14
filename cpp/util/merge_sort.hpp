template <typename T, typename INT_TYPE>
void merge_sort(T *values, INT_TYPE n, T *temp){
    if (n > 1){
        INT_TYPE m = n/2, i = 0, j = m, k = 0;
        merge_sort(values, m, temp);
        merge_sort(values + m, n - m, temp);
        while (i < m && j < n){
            temp[k++] = values[i] < values[j] ? values[i++] : values[j++];
        }
        while (i < m) temp[k++] = values[i++];
        for (i = 0; i < k; i++) values[i] = temp[i];
    }
}
